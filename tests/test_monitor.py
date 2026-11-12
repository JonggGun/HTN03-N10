"""Headless serial/GUI integration test, no board or pyserial needed."""
import os, sys, tempfile
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch
os.environ['MPLBACKEND']='Agg'
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'pc'))
import monitor
class FakeSerial:
    def __init__(self,*args,**kwargs):
        self.chunks=[b'THR,1000,1800,2600\n1200,1\nwrong\n', b'OK\n', b'THR,1100,1800,2600\n2500,2\n']
        self.writes=[];self.closed=False
    @property
    def in_waiting(self): return len(self.chunks[0]) if self.chunks else 0
    def read(self,n): return self.chunks.pop(0) if n else b''
    def write(self,data): self.writes.append(data);return len(data)
    def close(self): self.closed=True
fake=FakeSerial()
class FakeAnimation:
    def __init__(self,fig,func,**kwargs):
        for i in range(5):func(i)
        self.event_source=None  # Matplotlib can clear this during window close.
with tempfile.TemporaryDirectory() as tmp:
    with patch.dict(sys.modules,{'serial':SimpleNamespace(Serial=lambda *a,**k:fake, SerialException=OSError)}), \
         patch.object(sys,'argv',['monitor.py','--port','COM_TEST']), \
         patch.object(monitor,'__file__',str(Path(tmp)/'monitor.py')), \
         patch.object(monitor,'FuncAnimation',FakeAnimation), \
         patch.object(monitor.plt,'show',lambda:None):
        monitor.main()
    log=list((Path(tmp)/'logs').glob('*.csv'))[0].read_text()
    assert '1200,1,1000,1800,2600,STM32' in log
    assert '2500,2,1100,1800,2600,STM32' in log
    assert fake.writes.count(b'GET\n')>=2
    assert fake.closed
print('Monitor integration PASS: serial data, OK->GET, thresholds, CSV, shutdown')
