# Trạng thái kiểm thử và việc cần làm trên kit

## Đã thực hiện trong môi trường tạo mã

- Host GCC: biên dịch C99 với Wall/Wextra/Werror; test logic chạy với AddressSanitizer và UndefinedBehaviorSanitizer. LeakSanitizer tắt vì môi trường không hỗ trợ.
- Test các biên T1/T2/T3, giảm qua hysteresis, nhảy nhiều mức, trung bình trượt khi chưa đủ mẫu và khi vòng đệm đầy; lệnh sai không sửa ngưỡng, lệnh dài không tràn số.
- Python unittest: 4 nhóm kiểm tra dữ liệu, ngưỡng, dữ liệu lỗi và OK/ERR.
- GNU Arm GCC 13.2.1: compile và link 17 file C cho Cortex-M3 với Wall/Wextra/Werror, không cảnh báo. text=11696 byte, data=84 byte, bss=10576 byte trong bản GNU. Flash tải khoảng 11780 byte; RAM tĩnh 10660 byte, trong đó có heap RTOS 10240 byte; thêm 1024 byte dự phòng main/ISR stack. Đây không phải số build Keil.
- Giao diện matplotlib: chạy dữ liệu mô phỏng ở chế độ headless, tạo chart và CSV. Chưa xác minh USB-UART vật lý.
- Test tích hợp Python dùng serial giả: đọc dữ liệu, OK dẫn tới GET, cập nhật ngưỡng, ghi CSV và đóng giao diện khi timer đã bị hủy.

## Chưa thực hiện

- Mở/build bằng Keil ARM Compiler 6; cần xác nhận cấu hình compiler, MicroLIB, scatter và pack trên máy của bạn.
- Nạp STM32F103C8T6 thật, đo ADC, kiểm tra clock/UART/I2C, OLED, buzzer.
- Đo high-water mark và free heap khi chạy thật, kiểm tra thời gian thực, độ ổn định và hiệu chuẩn MQ-135.

## Checklist để điền kết quả thật

| Mục | Kỳ vọng | Kết quả / người thử / ngày |
|---|---|---|
| Build Keil | Không lỗi/cảnh báo; nằm trong 64 KB Flash và 20 KB RAM | Chưa thử |
| PA0 | Điện áp trong khoảng 0–VDDA; ADC biến đổi đúng | Chưa thử |
| OLED | Hiện ADC, mức, 3 ngưỡng | Chưa thử |
| UART | Nhận dữ liệu 115200 8N1, GET và SET đúng | Chưa thử |
| Hysteresis | Mức 3 giữ ở 2550 và xuống khi dưới 2550 với T3=2600 | Chưa thử |
| LED/buzzer | Bật ở mức 3, tắt ở mức thấp hơn | Chưa thử |
| SET lỗi | ERR, ngưỡng không đổi | Chưa thử |
| Reset | Trở lại ngưỡng mặc định | Chưa thử |
| Không OLED | ADC/UART vẫn chạy, oled_errors tăng | Chưa thử |
| Chạy lâu | Không tràn stack/queue; heap và stack còn dư | Chưa thử |
| MQ-135 | Có thời gian làm nóng và dữ liệu nền thực nghiệm | Chưa thử |

## Chạy lại test

Tại gốc project, nếu có GCC và Python:

```bash
gcc -std=c99 -Wall -Wextra -Werror -Iapp/inc app/src/air_logic.c tests/test_logic.c -o air_logic_test
./air_logic_test
python tests/test_protocol.py
python tests/test_monitor.py
```

Build độc lập bằng GNU Arm (không thay thế build Keil):

```bash
python tests/build_gcc.py --prefix /duong/dan/bin/arm-none-eabi-
```

Các sản phẩm build không có trong ZIP. Không dùng log mô phỏng như bằng chứng đo thật.
