"""Independent compile/link validation; Keil remains the delivery target.
Usage: python tests/build_gcc.py --prefix /path/bin/arm-none-eabi-
"""
import argparse, subprocess
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--prefix',default='arm-none-eabi-');args=p.parse_args()
r=Path(__file__).resolve().parents[1];out=r/'build';out.mkdir(exist_ok=True)
incs=['bsp','app/inc','hal/inc','middlewares/FreeRTOS/include','middlewares/FreeRTOS/portable/GCC/ARM_CM3']
src=['main.c']+[str(x.relative_to(r)) for d in ['bsp','app/src','hal/src'] for x in sorted((r/d).glob('*.c'))]+['middlewares/FreeRTOS/'+x for x in ['tasks.c','queue.c','list.c','portable/GCC/ARM_CM3/port.c','portable/MemMang/heap_4.c']]
common=['-mcpu=cortex-m3','-mthumb','-std=c99','-Os','-g','-ffunction-sections','-fdata-sections','-Wall','-Wextra','-Werror']+['-I'+str(r/x) for x in incs]
objects=[]
for i,s in enumerate(src):
 o=out/f'{i}.o';subprocess.run([args.prefix+'gcc',*common,'-c',str(r/s),'-o',str(o)],check=True);objects.append(str(o))
elf=out/'air_quality.elf'
subprocess.run([args.prefix+'gcc','-mcpu=cortex-m3','-mthumb','--specs=nano.specs','--specs=nosys.specs','-nostartfiles',*objects,'-T'+str(r/'tests/stm32f103.ld'),'-Wl,--gc-sections,-Map='+str(out/'air_quality.map'),'-o',str(elf)],check=True)
subprocess.run([args.prefix+'size',str(elf)],check=True)
