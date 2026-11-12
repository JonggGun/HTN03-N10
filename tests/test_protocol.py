import sys, unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'pc'))
from protocol import parse_line
class ProtocolTests(unittest.TestCase):
    def test_data(self):
        self.assertEqual(parse_line('4095,3\r\n'),('data',4095,3))
        self.assertEqual(parse_line('0,0'),('data',0,0))
    def test_limits(self):
        self.assertEqual(parse_line('THR,1000,1800,2600'),('thresholds',1000,1800,2600))
        for s in ['THR,0,1,2','THR,2,1,3','THR,1,2,4096']:
            self.assertIsNone(parse_line(s))
    def test_bad(self):
        for s in ['4096,0','1,4','abc,0','1,2,3','', 'ERR,junk']:
            self.assertIsNone(parse_line(s))
    def test_reply(self):
        self.assertEqual(parse_line('OK\r\n'),('OK',))
        self.assertEqual(parse_line('ERR'),('ERR',))
if __name__=='__main__': unittest.main()
