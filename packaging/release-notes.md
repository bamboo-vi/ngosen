Thay đổi của bản này: xem [CHANGELOG.md](https://github.com/ngosen/ngosen/blob/main/CHANGELOG.md).

## Chọn tệp theo bản phân phối

| Bản phân phối           | Tệp                                              | Mức đã thử                                                                   |
| ----------------------- | ------------------------------------------------ | ---------------------------------------------------------------------------- |
| Fedora 44               | `fcitx5-ngosen-*.fc44.x86_64.rpm`                | dùng hằng ngày (KDE Plasma Wayland)                                          |
| Fedora 43               | `fcitx5-ngosen-*.fc43.x86_64.rpm`                | chỉ dựng                                                                     |
| Arch, CachyOS           | `fcitx5-ngosen-*-x86_64.pkg.tar.zst`             | gõ thử 5 app ở chế độ Gõ Sen trên máy ảo CachyOS (Hyprland)                  |
| Ubuntu 26.04            | `…_resolute_amd64.deb`                           | gõ thử 9 app trên máy ảo (GNOME Wayland), với bản tự dựng                    |
| Ubuntu 24.04            | `fcitx5-ngosen_*_noble_amd64.deb`                | gõ thử 7 app trên máy ảo Linux Mint 22 (cùng nền 24.04), với bản tự dựng     |
| Debian 13               | `…_trixie_amd64.deb`                             | gõ thử 6 app trên máy ảo MX 25 (cùng nền Debian 13)                          |
| Ubuntu 22.04, Debian 12 | `…_jammy_amd64.deb`, `…_bookworm_amd64.deb`      | chỉ dựng                                                                     |
| openSUSE Tumbleweed     | `fcitx5-ngosen-*.opensuse-tumbleweed.x86_64.rpm` | chỉ dựng                                                                     |
| Kali Rolling            | `…_forky_amd64.deb` (lệnh cài tự chọn)           | lệnh cài chạy trên máy ảo Kali 2026.2 (Xfce X11)                             |
| Máy ARM 64-bit          | `…_arm64.deb`, `fcitx5-ngosen-*.aarch64.rpm`     | ansidev cài và chạy được trên Ubuntu 26.04 (máy ảo VMware); còn lại chỉ dựng |

"Chỉ dựng" nghĩa là gói dựng được và qua bộ kiểm lúc dựng, chưa ai gõ thử trên bản phân phối đó.

Cột "Mức đã thử" là lần gõ thử bản 0.5. Các bản sửa bảng tính mới trong 1.0 được gõ thử trên máy ảo CachyOS
(KDE Wayland): LibreOffice Calc, WPS Office và Google Sheets trong Firefox. Bản 1.1 đổi tên các file cài
vào máy; lần cập nhật từ 1.0.0-1 được thử trên máy ảo Linux Mint 22 (Cinnamon X11): cấu hình cũ được chép
sang, bộ gõ vẫn nằm trong danh sách, gõ ra đúng chữ.

Bản 1.2 thay cửa sổ cài đặt. Lưu và Hoàn tác qua fcitx5 thật được thử trên máy ảo Linux Mint 22 (Cinnamon
X11), CachyOS (KDE Wayland) và Ubuntu 26.04 (GNOME Wayland). Gõ thử bản 1.2 trên máy ảo CachyOS (KDE
Wayland) trong GNOME Text Editor: `tieengs vieetj roofi ngafy dduowcj` ra đúng chữ. Bản sửa lỗi đảo chữ
trong gnome-terminal được thử bằng client D-Bus giả lập và gõ tay trong gnome-terminal: không còn đảo chữ.

Bản 1.2.1 chỉ bỏ trang Ứng dụng khỏi cửa sổ cài đặt, bộ gõ không đổi. Cửa sổ 4 trang được mở trên máy ảo
CachyOS (KDE Wayland), cả giao diện sáng và tối.

Bản 1.2.2 sửa nhãn cạnh con trỏ và lỗi mất dấu trên thanh địa chỉ Edge. Lỗi Edge được tái hiện bằng test
dựng lại từ log gõ phím trên Fedora 44 (KDE Wayland), rồi gõ tay lại trên Edge sau khi sửa: ra đúng `ngó`, cạnh con trỏ hiện `vi`.

Bản 1.3.0 thêm tùy chọn Backspace xóa dấu thanh, được gõ tay trên Fedora 44 (KDE Wayland) với bản dựng từ
nhánh `next`. Icon khay mới được xem trên máy ảo Linux Mint 22.3 (Cinnamon) và MX Linux 25.3 (Xfce). Gói
Debian 13 được cài trên máy ảo MX Linux 25.3: Telex `vieejt` ra `việt` trong terminal GTK. Lần cập nhật từ
1.0.0-1 rồi chạy `fcitx5 -rd` cũng được thử trên máy ảo MX Linux 25.3: Ngó Sen vẫn nằm trong danh sách bộ gõ.

Bản 1.4.0 chủ yếu đổi lệnh cài một dòng. Lệnh cài được chạy trên máy ảo Ubuntu 26.04 (GNOME Wayland),
CachyOS (GNOME 51), Linux Mint (Cinnamon X11) và MX Linux 25 (Xfce X11) với bản dựng từ nhánh `next`: sau khi
đăng nhập lại, Ctrl+Shift rồi Telex `tieengs vieetj` ra `tiếng việt` trong ô chữ GTK3, 3/3 lần trên mỗi máy.
Lệnh gỡ được thử trên Ubuntu 26.04 (IBus chạy lại) và MX Linux 25.

Bản 1.4.1 chỉ đổi lệnh cài, bộ gõ giữ nguyên như 1.4.0. Lệnh cài trên KDE được chạy trên máy ảo CachyOS
(Plasma 6.7 Wayland) với tài khoản chưa cấu hình bộ gõ: sau khi đăng nhập lại, Ctrl+Shift rồi Telex
`tieengs vieetj` ra `tiếng việt` 3/3 lần trong ô chữ Qt6 và 3/3 lần trong ô chữ GTK3; trước khi sửa, ô chữ
Qt6 ra chữ thô. Với gói 1.4.0, Chrome 155, VS Code, Discord, Firefox và GNOME Text Editor (Flatpak) gõ đúng
trên máy ảo Ubuntu 26.04 (GNOME Wayland); Chrome ép chạy qua XWayland sai 5/30 lần.

Bản 1.4.2 là bản sửa lỗi, gõ thử với bản dựng từ nhánh `release/1.4.2`. Ô hỏi của Gemini trong Chromium trên
máy ảo CachyOS (Plasma Wayland): `ddown` ở đầu ô trống ra `đơn` 3/3 câu (trước khi sửa ra `n`). Trên Sway
(CachyOS), dòng `dduwowngf tieengs vieetj…` ra đúng `đường tiếng việt…` trong ô chữ GTK3 và Firefox. Trên máy
ảo Ubuntu 26.04 (GNOME Wayland), `ddwowngf tieengs vieetj` ra `đường tiếng việt` 2/2 lần trong Ptyxis và 2/2
lần trong GNOME Console. Gói ARM 64-bit được ansidev cài và chạy trên Ubuntu 26.04 (máy ảo VMware) từ bản thử.

Lõi ghép dấu viết bằng Rust có trong gói Fedora, Arch, openSUSE và Ubuntu 24.04 trở lên. Debian 12, 13 và
Ubuntu 22.04 vẫn dùng lõi Go, vì Rust có sẵn ở đó quá cũ. Hai lõi gõ ra chữ như nhau.

Cài xong, hoặc mỗi lần cập nhật, khởi động lại fcitx5:

```
fcitx5 -rd
```

KDE Wayland: đăng xuất rồi đăng nhập lại, vì KDE tự mở fcitx5 và lệnh trên không thay được.

Ubuntu 26.04: xem mục "bật extension sửa lỗi gõ" trong [README](https://github.com/ngosen/ngosen#cài).

Đối chiếu tệp tải về với `SHA256SUMS`: `sha256sum -c SHA256SUMS --ignore-missing`.
