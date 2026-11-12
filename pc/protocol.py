"""Pure parsing helpers shared by the GUI and offline tests."""
def parse_line(line):
    parts = line.strip().split(',')
    try:
        if len(parts) == 2:
            adc, level = map(int, parts)
            if 0 <= adc <= 4095 and 0 <= level <= 3:
                return ('data', adc, level)
        elif len(parts) == 4 and parts[0] == 'THR':
            t = tuple(map(int, parts[1:]))
            if 0 < t[0] < t[1] < t[2] <= 4095:
                return ('thresholds', *t)
        elif line.strip() in ('OK', 'ERR'):
            return (line.strip(),)
    except ValueError:
        pass
    return None
