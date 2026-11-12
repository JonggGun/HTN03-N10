# Hướng dẫn làm và nộp bài bằng Keil C

Bộ project này thực hiện toàn bộ đề cương HTN03-N10 trên STM32F103C8T6. Bạn cần tự build bằng Keil, đấu dây, nạp kit và ghi lại kết quả thực nghiệm. Những kết quả đó chưa có trong gói tải về.

## 1. Phần cứng và lựa chọn đã dùng

- STM32F103C8T6, Flash 64 KB, RAM 20 KB; thường là board Blue Pill.
- MQ-135 dạng module có chân AO; dùng ADC thô 0–4095, không tính ppm/AQI.
- OLED **SSD1306 I2C 128×64**, địa chỉ 7 bit mặc định **0x3C**. Module SH1106 hoặc OLED 128×32 cần đổi driver.
- USB-UART mức logic **3,3 V**, 115200 baud, 8N1; không dùng cổng RS-232 ±12 V.
- LED ngoài và buzzer **active** được điều khiển mức cao. Buzzer passive cần PWM, chưa dùng trong thiết kế này.
- ST-Link dùng để nạp/debug; USB-UART dùng để truyền dữ liệu. Hai thiết bị có vai trò khác nhau.

### Bảng nối dây

| Thiết bị | Chân thiết bị | Nối tới |
|---|---|---|
| MQ-135 | VCC | Nguồn 5 V đủ dòng theo module |
| MQ-135 | GND | GND chung |
| MQ-135 | AO | Qua R1 10 kΩ tới PA0; R2 15 kΩ từ PA0 xuống GND |
| MQ-135 | DO | Không nối |
| OLED | VCC | 3,3 V với module hỗ trợ điện áp này |
| OLED | GND | GND chung |
| OLED | SCL | PB6 / I2C1_SCL |
| OLED | SDA | PB7 / I2C1_SDA |
| USB-UART | RXD | PA9 / USART1_TX |
| USB-UART | TXD | PA10 / USART1_RX |
| USB-UART | GND | GND chung |
| LED ngoài | Anode qua điện trở 330 Ω | PB12 |
| LED ngoài | Cathode | GND |
| Buzzer active | Điều khiển transistor | PB13 qua điện trở base/gate phù hợp |
| ST-Link | SWDIO | PA13 |
| ST-Link | SWCLK | PA14 |
| ST-Link | GND | GND chung |
| ST-Link | VTref, nếu có | 3,3 V để nhận biết điện áp đích |

Chia áp R1=10 kΩ và R2=15 kΩ cho V(PA0)=0,6×V(AO); AO=5 V thì PA0≈3 V. Đo lại bằng đồng hồ trước khi nối PA0. ADC chỉ nhận trong khoảng GND đến VDDA, khoảng 3,3 V trên board; không nối thẳng AO 5 V vào PA0. Ngưỡng phải hiệu chuẩn lại theo mạch chia áp thực tế.

SDA/SCL cần pull-up lên **3,3 V**, thường 4,7 kΩ; kiểm tra module đã có chưa. Không dùng pull-up lên 5 V trong cấu hình này. Với buzzer dùng transistor NPN: emitter xuống GND, collector vào cực âm buzzer, cực dương tới nguồn phù hợp, PB13 qua điện trở khoảng 1–4,7 kΩ vào base; thêm diode bảo vệ nếu tải có tính cảm. Không cấp dòng buzzer trực tiếp từ GPIO. Một nguồn phù hợp cho board và cảm biến; không chập các đường cấp nguồn từ USB-UART, ST-Link và nguồn ngoài với nhau.

Để chạy từ Flash, BOOT0 đặt 0, nhấn Reset sau khi nạp. PC13 là LED lỗi nghiêm trọng nếu board có LED tích hợp active-low; LED cảnh báo chất lượng không khí là PB12.

## 2. Mở và build Keil

1. Giải nén ZIP vào `C:\HTN\AirQuality_STM32F103C8T6` hoặc đường dẫn ngắn không dấu.
2. Mở `keil\AirQuality.uvprojx` bằng Keil uVision 5.
3. Nếu Keil yêu cầu gói chip, cài **Keil STM32F1xx_DFP** trong Pack Installer. Device là STM32F103C8; không chọn chip RAM/Flash lớn hơn.
4. Bấm **Options for Target** (Alt+F7). Tab **Target**: chọn bản **ARM Compiler 6** đã cài, bật **Use MicroLIB**, nhập Xtal 8 MHz. Firmware thực tế dùng HSI nội 8 MHz; trường Xtal ở đây phục vụ cấu hình IDE.
5. Tab **C/C++ (AC6)**: C99, include paths đã có trong project. Không chọn Compiler 5 cho bộ startup/port này.
6. Tab **Linker**: dùng scatter file `air_quality.sct` trong thư mục keil, không tự sinh memory layout khác. Flash bắt đầu 0x08000000, kích thước 0x10000; RAM bắt đầu 0x20000000, kích thước 0x5000. 1 KB trên cùng dành cho main/interrupt stack; 256 byte ngay dưới dành cho heap MicroLIB, tách biệt với heap FreeRTOS 10 KB.
7. Tab **Output**: bật Create HEX File. Bấm F7 để Build. Project gồm 17 file C; FreeRTOS đã có trong ZIP.
8. Không thêm `startup_stm32f10x_md.s`, `system_stm32f10x.c`, thư viện HAL của CubeMX hoặc một bản FreeRTOS khác vào cùng target. Bộ này đã có startup, bảng vector và driver riêng.

Nếu Keil báo compiler version chưa cài: vào Target chọn một bản ARM Compiler 6 đang có, hoặc cài Compiler 6 rồi chọn lại. Nếu báo thiếu device pack: cài STM32F1xx_DFP. Nếu báo trùng Reset_Handler/SysTick_Handler: kiểm tra có thêm startup/RTOS thứ hai hay không. Nếu lỗi linker do scatter file không tìm thấy, dùng Browse chọn lại `keil\air_quality.sct`.

## 3. Nạp bằng ST-Link

1. Options for Target → Debug → chọn ST-Link Debugger → Settings → Port **SW**, có thể bắt đầu với tốc độ 1 MHz.
2. Utilities → chọn ST-Link để nạp Flash.
3. Flash Download → Add → chọn thuật toán cho **STM32F10x Medium Density / 64 KB Flash**, địa chỉ 0x08000000. Tên hiển thị có thể khác theo phiên bản pack.
4. Build F7 → Download F8 → Reset board. Nếu cần, bật Reset and Run.
5. Debug để xem biến `g_diag`: các trường `adc_errors`, `sensor_dropped`, `uart_rx_dropped`, `uart_tx_errors`, `oled_errors`; `sensor_stack`, `analysis_stack`, `display_stack` là số **word stack còn trống ít nhất**, không phải byte; `free_heap` là số byte heap RTOS còn trống.

ST-Link không nhất thiết tạo COM. Cổng COM dùng cho biểu đồ phải là USB-UART nối PA9/PA10. Khi dùng debugger, tránh dừng CPU trong lúc PC đang gửi lệnh liên tục.

## 4. Chạy Python trên Windows

Cài Python có pip rồi mở terminal tại thư mục `pc`:

```powershell
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
.\.venv\Scripts\python.exe monitor.py --port COM5
```

Thay COM5 bằng cổng trong Device Manager → Ports (COM & LPT). Nếu máy dùng lệnh `py`, có thể chạy `py -m venv .venv` ở bước đầu. Đóng PuTTY, Serial Monitor hoặc chương trình khác đang giữ COM trước khi chạy.

Nếu chưa nối phần cứng, kiểm tra giao diện bằng:

```powershell
.\.venv\Scripts\python.exe monitor.py --demo
```

Demo chỉ tạo dữ liệu giả và tắt gửi lệnh; CSV ghi rõ DEMO. Không dùng dữ liệu đó làm kết quả đo của MQ-135.

Giao diện hiển thị khoảng 60 giây gần nhất, ADC, mức hiện tại, ba đường ngưỡng và bốn vùng màu. Dữ liệu CSV được lưu tự động tại `pc/logs`, gồm thời gian, ADC, mức, ngưỡng và nguồn dữ liệu. Nếu chưa đồng bộ được ngưỡng, các cột ngưỡng trong CSV để trống; đường ngưỡng trên màn hình tạm dùng mặc định. Đóng cửa sổ để kết thúc và đóng file log.

Trên Ubuntu có thể dùng `python3 monitor.py --port /dev/ttyUSB0` sau khi cài dependencies và cấp quyền truy cập serial phù hợp. Trên Windows không dùng tên `/dev/ttyUSB0`.

## 5. Định dạng UART và đổi ngưỡng

STM32 gửi dòng dữ liệu **`ADC,LEVEL`**, ví dụ `1820,2`. LEVEL là số 0, 1, 2 hoặc 3. Mỗi dòng kết thúc CRLF. Lệnh từ máy tính kết thúc LF; CRLF cũng được chấp nhận.

| Gửi tới STM32 | Phản hồi |
|---|---|
| `GET` | `THR,650,850,1100` theo ngưỡng thực tế |
| `SET T1 1100` | `OK` nếu hợp lệ; ngưỡng được áp dụng |
| `SET T2 1900` | `OK` nếu hợp lệ |
| `SET T3 2700` | `OK` nếu hợp lệ |
| `SET T2 500` | `ERR` nếu không thỏa T1 < T2 |

Trong ô Command của Python, nhập lệnh rồi Enter hoặc Send. Khi nhận OK, Python tự gửi GET để lấy ngưỡng thật từ MCU. Quy tắc là **0 < T1 < T2 < T3 ≤ 4095**. Gửi mỗi lệnh rồi chờ phản hồi, không dán hàng trăm lệnh cùng lúc.

Nếu muốn tăng cả ba ngưỡng vượt ngưỡng cũ, nên tăng T3 rồi T2 rồi T1; khi hạ cả ba thì hạ T1 rồi T2 rồi T3 để mỗi bước vẫn hợp lệ. Ngưỡng chỉ lưu trong RAM: reset hoặc mất nguồn sẽ về 650/850/1100. Đề cương không yêu cầu lưu Flash nên bộ này không ghi Flash.

Phân mức khi bắt đầu hoặc vừa đổi ngưỡng: ADC<T1 là Tốt; T1≤ADC<T2 là Trung bình; T2≤ADC<T3 là Kém; ADC≥T3 là Nguy hại. Khi đang chạy, mức tăng ngay tại ngưỡng và giảm khi ADC **nhỏ hơn ngưỡng trừ 50**. Ví dụ đang mức 3: 2550 vẫn giữ mức 3, 2549 xuống mức 2. Với ngưỡng rất nhỏ, hysteresis được giới hạn để ADC=0 vẫn có thể trở lại mức 0. Vì hysteresis, đôi lúc màu vùng theo ADC và mức hiện tại có thể khác nhau gần biên.

## 6. Cấu trúc chương trình

| Thành phần | Hoạt động |
|---|---|
| AirSensorTask | Mỗi 100 ms đọc ADC1 kênh 0; trung bình trượt 10 mẫu; gửi queue 8 phần tử |
| AnalysisTask | Nhận mẫu, sở hữu ngưỡng, xử lý UART, hysteresis và PB12/PB13 |
| DisplayTask | Nhận trạng thái mới nhất từ queue 1 phần tử, gửi UART và cập nhật OLED |
| ISR USART1 | Chỉ lấy byte và dùng xQueueSendFromISR; không xử lý lệnh dài trong ISR |
| app_ipc | Tạo queue dùng chung và chứa thống kê chẩn đoán |
| main | Khởi tạo BSP/HAL/IPC, tạo 3 task, chạy scheduler |

Ưu tiên Sensor=3, Analysis=2, Display=1; stack lần lượt 192/384/384 word. Idle task của FreeRTOS là task hệ thống. Heap RTOS 10 KB dùng heap_4. ADC, I2C và UART TX có mutex; các lần đợi cờ ngoại vi đều có giới hạn. GPIO cảnh báo chỉ AnalysisTask ghi. Cấu hình này tránh OLED chậm làm chặn chu kỳ lấy mẫu; nếu queue sensor đầy, tăng bộ đếm lỗi thay vì đợi vô hạn.

Không có HAL_Delay hoặc delay_ms trong task. Đợi định kỳ dùng vTaskDelay/vTaskDelayUntil; các vòng chờ cờ ngoại vi có giới hạn trong driver. UART lỗi/tràn thì bỏ lệnh chưa hoàn chỉnh. Nếu OLED không nối, UART vẫn chạy và OLED được thử khởi tạo lại mỗi khoảng 2 giây. Nếu I2C bị giữ thấp do phần cứng, cần sửa dây/nguồn rồi reset; driver không thực hiện phát xung khôi phục bus.

## 7. Kiểm tra trên kit trước khi nộp

1. Build Keil và ghi lại số error/warning, Code/RO-data/RW-data/ZI-data trong log.
2. Kiểm tra điện áp PA0 nằm trong 0–3,3 V. Có thể dùng biến trở 10 kΩ nối giữa 3,3 V và GND, chân giữa vào PA0 để kiểm tra ADC trước khi gắn MQ-135.
3. Chạy Python, kiểm tra GET nhận đúng ba ngưỡng và ADC thay đổi theo biến trở. Do lọc 10 mẫu, đáp ứng đầy đủ mất khoảng 1 giây.
4. Qua từng ngưỡng, đối chiếu OLED, Python và LED/buzzer; thử giảm qua T3−50 để kiểm tra hysteresis.
5. Thử lệnh đúng, lệnh sai, ngưỡng ngoài miền; ERR phải giữ nguyên ngưỡng cũ.
6. Ngắt OLED khi đã tắt nguồn, bật lại rồi kiểm tra UART/ADC vẫn hoạt động. Nối lại đúng dây và reset để thử OLED.
7. Quan sát `g_diag` sau khi chạy lâu và đổi ngưỡng nhiều lần: stack còn dư, heap còn dư, không tràn queue trong vận hành bình thường. Chụp ảnh kết quả thật và lưu CSV.
8. Lắp MQ-135, làm nóng theo datasheet của đúng module, đợi nền ổn định rồi hiệu chuẩn lại. Không báo giá trị ADC như chỉ số an toàn sức khỏe hoặc AQI chuẩn.

## 8. Đưa code lên Git theo quy trình nhóm

### 8.1 Chuẩn bị

Bạn chưa cung cấp URL repo, nên những lệnh sau dùng `<URL_REPO>` và `<THU_MUC_REPO>` để bạn thay bằng giá trị thật. Dùng Git Bash ở Windows. Các lệnh này không tự động được chạy trên tài khoản của bạn.

Nếu nhóm chưa có repo: trưởng nhóm tạo repo với README và `.gitignore`, khung thư mục, sau đó tạo `develop` từ `main`. Đây là bước khởi tạo; các thay đổi chức năng về sau đi qua PR, không commit trực tiếp vào main.

```bash
git clone <URL_REPO>
cd <THU_MUC_REPO>
git fetch origin
git switch develop
git pull --ff-only origin develop
```

Nếu develop chưa tồn tại, trưởng nhóm tạo một lần:

```bash
git switch main
git pull --ff-only origin main
git switch -c develop
git push -u origin develop
```

Để thư mục ZIP đã giải nén **bên ngoài repo clone**, rồi chép đúng các file của từng giai đoạn vào repo. Không chép thư mục `.git`, không chép toàn bộ ZIP hoặc file build lên repo. Nếu repo đã có code khác, so sánh và ghép từng thay đổi thay vì ghi đè. Khung/API do nhóm đang dùng là điểm cần thống nhất trước khi thay thế.

### 8.2 Nhánh nền tảng và HAL

Nhánh này dành cho việc chuẩn bị bộ nền tảng mới; nếu leader đã chuẩn bị HAL/FreeRTOS, dùng khung của leader và chỉ chuyển phần cần thiết.

```bash
git switch develop
git pull --ff-only origin develop
git switch -c feature/platform-hal
```

Chép `bsp`, `hal`, `middlewares`, `.gitignore`; chép `app_cfg.h`, `FreeRTOSConfig.h`, `air_logic.h`, `app_ipc.h` vào `app/inc`; chép `air_logic.c`, `app_ipc.c` vào `app/src`.

```bash
git add bsp hal middlewares .gitignore
git add app/inc/app_cfg.h app/inc/FreeRTOSConfig.h app/inc/air_logic.h app/inc/app_ipc.h
git add app/src/air_logic.c app/src/app_ipc.c
git diff --cached --stat
git commit -m "feat(hal): add STM32F103 drivers and shared FreeRTOS interfaces"
git push -u origin feature/platform-hal
```

Mở PR với **base: develop**, **compare: feature/platform-hal**. Ghi rõ đây là nền tảng, chưa chứa main ghép toàn bộ. Test driver độc lập cần harness của nhóm hoặc thử bản tích hợp trong thư mục riêng; không tuyên bố nhánh trung gian là firmware hoàn chỉnh. Nhờ thành viên khác review/leader merge rồi mới tiếp tục.

### 8.3 Ba nhánh task

Sau khi nhánh trước được merge, luôn cập nhật develop rồi tạo nhánh tiếp theo. Với sensor:

```bash
git switch develop
git pull --ff-only origin develop
git switch -c feature/task-sensor
```

Chép `app/inc/task_sensor.h` và `app/src/task_sensor.c`:

```bash
git add app/inc/task_sensor.h app/src/task_sensor.c
git commit -m "feat(task-sensor): sample ADC and filter air readings"
git push -u origin feature/task-sensor
```

Mở PR `feature/task-sensor` → `develop`. Chờ merge. Lặp lại cùng quy trình với:

| Nhánh | File cần chép và git add | Commit message |
|---|---|---|
| feature/task-analysis | app/inc/task_analysis.h, app/src/task_analysis.c | feat(task-analysis): classify readings and handle threshold commands |
| feature/task-display | app/inc/task_display.h, app/src/task_display.c | feat(task-display): update OLED and publish UART readings |

Các nhánh task **không thay đổi main.c**. Muốn test riêng từng task, dùng project/harness cục bộ với mock queue, không đưa main test đó vào PR task. Bộ ZIP chứa main tích hợp hoàn chỉnh phục vụ bước sau.

### 8.4 Python, tài liệu và kiểm thử

Từ develop mới nhất, tạo `feature/pc-monitor`, chép `pc`, `docs`, `tests`, `README.md`, `HUONG_DAN.html`. Không chép `pc/logs`, `.venv`, `build`.

```bash
git switch develop
git pull --ff-only origin develop
git switch -c feature/pc-monitor
git add pc docs tests README.md HUONG_DAN.html
git diff --cached --stat
git commit -m "feat(pc-monitor): add UART plotting commands and project guide"
git push -u origin feature/pc-monitor
```

Mở PR vào develop rồi chờ review/merge. Script build ở tests sẽ cần main và project tích hợp để build toàn bộ; test logic và parser có thể chạy độc lập.

### 8.5 Tích hợp tại main.c và project Keil

Sau khi tất cả các PR trên đã merge:

```bash
git switch develop
git pull --ff-only origin develop
git switch -c integration/system-v1
```

Lúc này chép `main.c` và thư mục `keil` từ gói giải nén. Người phụ trách tích hợp kiểm tra lại `app_ipc.c`, cấu hình bộ nhớ, chân và clock. Build Keil, nạp kit và thực hiện checklist.

```bash
git add main.c keil
git diff --cached --stat
git commit -m "feat(integration): wire tasks and add Keil STM32F103 target"
git push -u origin integration/system-v1
```

Mở PR `integration/system-v1` → `develop`, đính kèm kết quả kiểm thử thật. Không đánh dấu ổn định khi chỉ build qua. Chỉ người phụ trách cấu hình thay đổi clock, chân và FreeRTOSConfig; không tự generate CubeMX đè bộ này.

### 8.6 Đồng bộ khi develop thay đổi

Chỉ làm khi working tree sạch, trên nhánh feature của chính mình:

```bash
git status
git fetch origin
git rebase origin/develop
```

Nếu conflict: mở từng file, chọn/ghép phần đúng, xóa các dấu conflict, rồi:

```bash
git add <FILE_DA_SUA>
git rebase --continue
```

Nếu chưa biết xử lý thì `git rebase --abort` để trở lại trước rebase, rồi trao đổi với người viết phần liên quan. Sau khi rebase thành công nhánh đã push của **chính mình**:

```bash
git push --force-with-lease origin <TEN_NHANH_CUA_BAN>
```

Không force push main/develop hoặc nhánh đang có nhiều người cùng làm. Nếu nhánh chưa từng push, dùng `git push -u origin <TEN_NHANH_CUA_BAN>`.

### 8.7 Release do trưởng nhóm thực hiện

Sau khi integration đã được duyệt, merge về develop; sau khi chạy ổn định trên kit, đưa develop vào main theo quyền/protection của repo và gắn tag v1.0.0. Nếu nhánh main được bảo vệ, tạo PR develop → main; không vượt qua branch protection. Đối với repo cho phép leader merge bằng CLI, sau khi review và kiểm tra remote:

```bash
git switch main
git pull --ff-only origin main
git fetch origin
git merge --no-ff origin/develop
git tag -a v1.0.0 -m "Release v1.0.0: verified air quality system"
git push origin main
git push origin v1.0.0
```

Chỉ tạo tag khi không có tag cùng tên và đã có kết quả phần cứng. Mỗi PR nên ghi: chức năng, file đổi, kiểm thử đã làm, kết quả, lỗi còn lại. Không ghi đã kiểm thử những mục chưa thực hiện.

## 9. File nào được up

Giữ `.c`, `.h`, `.uvprojx`, `.sct`, mã nguồn FreeRTOS và license, Python, requirements, tests và tài liệu. `.uvprojx` là project dùng chung nên **cần commit**. Không commit Objects, Listings, Debug, Release, build, `.axf`, `.hex`, `.elf`, `.o`, `.uvguix.*`, `.uvoptx`, `.venv` hoặc log đo tự sinh. Trước commit dùng `git status` và `git diff --cached --stat` để kiểm tra.

## 10. Nguồn kỹ thuật

- Đề cương HTN03-N10 và quy trình QuyTrinh-Git-2 do nhóm cung cấp.
- [STM32F103C8 datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf): bộ nhớ, chân và đặc tính điện.
- [STM32F1 RM0008 reference manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf): thanh ghi RCC, GPIO, ADC, USART, I2C.
- [FreeRTOS Kernel V11.1.0](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/V11.1.0): kernel, Cortex-M3 port và heap_4; bản quyền MIT giữ trong middlewares/FreeRTOS/LICENSE.md.
