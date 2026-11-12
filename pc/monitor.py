"""Run: python monitor.py --port COM5 ; offline UI: --demo"""
import argparse
import csv
import math
import time
from collections import deque
from datetime import datetime
from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import TextBox, Button
from protocol import parse_line

NAMES = ['Tot', 'Trung binh', 'Kem', 'Nguy hai']
COLORS = ['#9de0a0', '#fff19d', '#ffc078', '#ff9999']

def main():
    parser = argparse.ArgumentParser(description='STM32 air quality monitor; values are ADC, not ppm/AQI')
    parser.add_argument('--port', help='COM5 on Windows, /dev/ttyUSB0 on Linux')
    parser.add_argument('--demo', action='store_true', help='Simulated data, no MCU')
    args = parser.parse_args()
    if not args.demo and not args.port:
        parser.error('Use --port COM5 or --demo')
    ser = None
    if not args.demo:
        import serial
        try:
            ser = serial.Serial(args.port, 115200, timeout=0, write_timeout=0.2)
        except serial.SerialException as exc:
            parser.exit(1, f'Cannot open {args.port}: {exc}\n')
    logdir = Path(__file__).resolve().parent / 'logs'
    logdir.mkdir(exist_ok=True)
    logpath = logdir / (datetime.now().strftime('%Y%m%d_%H%M%S_%f') + ("_DEMO" if args.demo else "") + '.csv')
    output = logpath.open('w', newline='', encoding='utf-8')
    writer = csv.writer(output)
    writer.writerow(['time', 'elapsed_s', 'adc', 'level', 'T1', 'T2', 'T3', 'source'])
    samples = deque(maxlen=10000)
    thresholds = [650, 850, 1100]
    synced = args.demo
    rx = bytearray()
    start = time.monotonic()
    last_rx = start
    pending_until = 0.0
    retry_at = start
    spans, hlines = [], []
    fig, ax = plt.subplots(figsize=(10, 6))
    fig.subplots_adjust(bottom=0.25)
    ax.set(xlabel='Time (s, last 60 seconds)', ylabel='Filtered ADC (0-4095)', ylim=(0, 4095), xlim=(0, 60))
    ax.grid(alpha=0.2)
    plot, = ax.plot([], [], color='#164b96', lw=1.8)
    status = fig.text(0.1, 0.15, 'DEMO - simulated data' if args.demo else 'Waiting for GET response...')
    entry = TextBox(fig.add_axes([0.17, 0.06, 0.48, 0.055]), 'Command ', initial='SET T1 650')
    button = Button(fig.add_axes([0.70, 0.06, 0.17, 0.055]), 'Send')

    def redraw_limits():
        for artist in spans + hlines:
            artist.remove()
        spans.clear(); hlines.clear()
        edges = [0] + thresholds + [4095]
        for i in range(4):
            spans.append(ax.axhspan(edges[i], edges[i+1], color=COLORS[i], alpha=0.28))
        for t in thresholds:
            hlines.append(ax.axhline(t, color='gray', ls='--', lw=1))

    def send(command):
        nonlocal pending_until
        if ser is None:
            status.set_text('DEMO: commands disabled; connect a board to change thresholds')
            return
        try:
            ser.write((command.strip() + '\n').encode('ascii'))
            pending_until = time.monotonic() + 2.0
        except (OSError, UnicodeError) as exc:
            status.set_text(f'Send failed: {exc}')

    def submit(_):
        if pending_until and time.monotonic() < pending_until:
            status.set_text('Waiting for previous reply; retry shortly')
            return
        command = entry.text.strip()
        if command == 'GET':
            send(command); return
        p = command.split()
        if len(p) != 3 or p[0] != 'SET' or p[1] not in ('T1','T2','T3') or not p[2].isdigit():
            status.set_text('Use GET or SET T1 650 / SET T2 850 / SET T3 1100'); return
        send(command)
        status.set_text(f'Sent: {command}; waiting for MCU')

    button.on_clicked(submit)
    entry.on_submit(submit)
    redraw_limits()

    def update(_):
        nonlocal synced, pending_until, retry_at, last_rx
        now = time.monotonic(); elapsed = now-start
        lines = []
        if args.demo:
            adc = int(1950 + 1500*math.sin(elapsed/7))
            level = sum(adc >= t for t in thresholds)
            lines = [f'{adc},{level}']
        else:
            try:
                rx.extend(ser.read(min(4096, ser.in_waiting)))
            except OSError as exc:
                status.set_text(f'Serial disconnected: {exc}; close and restart')
                return
            while b'\n' in rx:
                line, _, rest = rx.partition(b'\n'); rx[:] = rest
                lines.append(line.decode('ascii', errors='replace'))
            if len(rx)>4096:
                rx.clear();status.set_text('Discarded oversized serial line')
            if not synced and now>=retry_at and not pending_until:
                send('GET');retry_at=now+2.5
        for line in lines:
            message = parse_line(line)
            if message is None: continue
            if message[0] == 'data':
                _, adc, level = message
                last_rx=now;samples.append((elapsed,adc,level))
                writer.writerow([datetime.now().isoformat(timespec='milliseconds'),f'{elapsed:.3f}',adc,level,
                    *(thresholds if synced else ['', '', '']), 'DEMO' if args.demo else 'STM32'])
            elif message[0] == 'thresholds':
                thresholds[:] = message[1:];synced=True;pending_until=0
                redraw_limits();status.set_text(f'Thresholds: {thresholds}; log: {logpath.name}')
            elif message[0] == 'OK':
                pending_until=0;synced=False;send('GET');retry_at=now+2.5
            elif message[0] == 'ERR':
                pending_until=0;status.set_text('MCU rejected command: require 0 < T1 < T2 < T3 <= 4095')
        if pending_until and now>pending_until:
            pending_until=0;status.set_text('Reply timeout; check wiring/baud, then send GET')
        while samples and samples[0][0]<elapsed-60:
            samples.popleft()
        if samples:
            plot.set_data([s[0] for s in samples],[s[1] for s in samples])
            label = 'DEMO | ' if args.demo else ''
            stale = ' | NO RECENT DATA' if now-last_rx>2 else ''
            ax.set_title(f'{label}ADC {samples[-1][1]} | {NAMES[samples[-1][2]]}{stale}')
        ax.set_xlim(max(0,elapsed-60),max(60,elapsed))
        output.flush()

    animation = FuncAnimation(fig, update, interval=100, cache_frame_data=False)
    try:
        plt.show()
    finally:
        if animation.event_source is not None:
            animation.event_source.stop()
        output.close()
        if ser is not None: ser.close()
    print(f'CSV saved: {logpath}')

if __name__ == '__main__':
    main()
