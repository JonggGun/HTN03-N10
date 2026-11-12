## Updated default thresholds

T1=650, T2=850, T3=1100 ADC counts. Reset restores these values. Experimental thresholds, not calibrated AQI/ppm limits. LED PB12; buzzer transistor PB13. Rebuild and download keil/AirQuality.uvprojx, then reset and send GET: THR,650,850,1100.

# Hệ thống giám sát chất lượng không khí STM32F103C8T6

Bộ mã nguồn cho đề tài nhóm 10 lớp 03: MQ-135 → ADC → FreeRTOS → OLED và UART → biểu đồ Python. Mở `HUONG_DAN.html` bằng Chrome/Edge để xem hướng dẫn đầy đủ, hoặc đọc `docs/HUONG_DAN.md`.

## Mở nhanh

1. Giải nén vào đường dẫn không dấu, ví dụ `C:\HTN\AirQuality_STM32F103C8T6`.
2. Mở `keil/AirQuality.uvprojx` trong Keil uVision 5, chọn **ARM Compiler 6**, bật **Use MicroLIB**, Build F7.
3. Chọn ST-Link, SWD và flash algorithm STM32F10x 64 KB; nạp rồi Reset. Không thêm startup hoặc system file khác.
4. Đấu dây theo bảng trong hướng dẫn. Đặc biệt AO của MQ-135 qua chia áp mới vào PA0.
5. Mở terminal ở `pc`, chạy `python -m pip install -r requirements.txt`, rồi `python monitor.py --port COM5` (thay COM5 bằng cổng thật).

## Nội dung

| Thư mục | Nội dung |
|---|---|
| bsp | Thanh ghi cần dùng, khởi động, cấu hình HSI 8 MHz |
| hal | ADC1, I2C1, USART1, GPIO và SSD1306 128×64 |
| app | Ba task, queues, lọc, lệnh UART và hysteresis |
| middlewares/FreeRTOS | FreeRTOS Kernel V11.1.0, các file cần thiết, MIT license |
| keil | Project ARM Compiler 6 và scatter file 64 KB Flash / 20 KB RAM |
| pc | Biểu đồ, gửi lệnh và lưu CSV |
| tests | Test logic C, parser Python, script kiểm tra build bằng GNU Arm |
| docs | Hướng dẫn và checklist thử trên kit |

Đây là bộ project độc lập mới vì chưa nhận repo hiện có của nhóm. Nếu nhóm đã có khung HAL/FreeRTOS khác, cần đối chiếu API trước khi ghép; không chép đè cấu hình chung. Ngưỡng mặc định chỉ là giá trị dự kiến của đề cương, không phải kết quả hiệu chuẩn. Các giá trị ADC không phải AQI hoặc ppm.

## Trạng thái kiểm tra

Đã kiểm tra logic C, parser Python, chạy giao diện bằng dữ liệu mô phỏng và cross-compile/link bằng GNU Arm GCC 13.2.1. Chưa chạy Keil ARM Compiler 6 hoặc nạp kit thực tế. Đọc `docs/KIEM_THU.md` trước khi báo cáo kết quả cho nhóm.
