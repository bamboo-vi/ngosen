Version 1.5.4-1 phát hành ngày 20/10/2026; 1.5.4-2 chỉ tăng lần đóng gói để đưa mẫu báo lỗi mới (#188) lên nhánh mặc định của GitHub, code chương trình giữ nguyên. Mẫu báo lỗi bắt đầu từ “Kiểm tra máy tôi…”, yêu cầu log từ “Lưu nhật ký gõ”, chế độ gõ, app và Wayland hay X11, kèm hướng dẫn lấy log fcitx5 hoặc backtrace khi fcitx5 bị treo. Tài liệu `docs/typing-without-preedit.md` nói rõ nhận định về surrounding text của Chromium chỉ áp dụng cho GTK IM (#184) và liệt kê các báo lỗi upstream do người khác gửi (#186). Ai đã cài 1.5.4-1 không cần cập nhật, vì lên 1.5.4-2 không thay đổi gì khi gõ.

Version 1.5.4 sửa lỗi Gõ Sen trong Chrome/Chromium trên Wayland: bình luận TikTok không còn để nguyên `ddungs rooif` thay vì `đúng rồi`, từ đầu ô Draft.js ghép dấu khi chữ gợi ý biến mất. Chrome trên GNOME cũng sửa lỗi mất chữ trong bình luận TikTok và ô Draft.js thường (`xin chào` thành `xin ch`, `đẹp` thành `ẹp`): Ngó Sen giữ từ khi ô mất focus rồi có lại ngay, chờ trang vẽ lại ô mới commit. Tuy vậy, từ đầu có dấu thanh gõ ngay sau khi bấm vào ô Draft.js trong Chrome trên GNOME đôi lúc vẫn mất chữ; các từ sau bình thường. Macro `e` → `em` giữ dấu cách, nên `e ko` ra `em không` thay vì `emkhông`. README thêm hướng dẫn dùng repo để update bằng dnf, apt, pacman hoặc zypper; tài liệu “Cách người Việt gõ” giải thích kiểu gõ không preedit. Chi tiết ở [CHANGELOG.md](https://github.com/ngosen/ngosen/blob/main/CHANGELOG.md).

## Cài và cập nhật

Một lệnh cho mọi distro. Lệnh tự chọn đúng gói và hỏi lại trước khi cài:

```
curl -fsSL https://raw.githubusercontent.com/ngosen/ngosen/main/install.sh | bash
```

Cài lần đầu thì đăng xuất rồi đăng nhập lại, sau đó bấm Ctrl+Space để bật tiếng Việt.

Cập nhật thì chạy lại đúng lệnh đó, rồi khởi động lại fcitx5 bằng `fcitx5 -rd`. Trên KDE Wayland thì đăng xuất rồi đăng nhập lại thay cho lệnh này, vì fcitx5 bật bằng `fcitx5 -rd` không gõ được trong app Qt như Kate, Dolphin.

## Tải gói bằng tay

Chọn file theo distro:

- Fedora 44, 43: `fcitx5-ngosen-*.fc44.x86_64.rpm`, `fcitx5-ngosen-*.fc43.x86_64.rpm`
- Arch, CachyOS: `fcitx5-ngosen-*-x86_64.pkg.tar.zst`
- openSUSE Tumbleweed: `fcitx5-ngosen-*.opensuse-tumbleweed.x86_64.rpm`
- Ubuntu 26.10, 26.04, 24.04, 22.04: `…_stonking_amd64.deb`, `…_resolute_amd64.deb`, `…_noble_amd64.deb`, `…_jammy_amd64.deb`
- Debian 13, 12: `…_trixie_amd64.deb`, `…_bookworm_amd64.deb`
- Kali Rolling: `…_forky_amd64.deb`
- Máy ARM 64-bit: file cùng tên với đuôi `_arm64.deb`, hoặc `.aarch64.rpm` cho Fedora

Cài gói bằng tay trên Ubuntu 26.04 thì bật thêm [extension sửa lỗi gõ](https://github.com/ngosen/ngosen#ubuntu-2604-extension-sửa-lỗi-gõ).

Đối chiếu file tải về với `SHA256SUMS`: `sha256sum -c SHA256SUMS --ignore-missing`.

## Đã test những gì

Version 1.5.4-2 không được gõ thử lại trên VM; các test gõ của 1.5.4-1 dưới đây vẫn áp dụng vì code chương trình giống hệt 1.5.4-1.

Ngày 20/10/2026, bản build từ nhánh `patch` (cùng code chương trình với 1.5.4-1), chép đè lên gói đã cài trên máy ảo, được test bằng Gõ Sen, Telex. Gõ `tieengs vieetj xin chaof cacs banj hoom nay trowif ddepj quas ddungs rooif` → `tiếng việt xin chào các bạn hôm nay trời đẹp quá đúng rồi` trên hai trang test: ô Draft.js thường và ô vẽ lại sau mỗi phím.

Ubuntu 26.04 (GNOME Wayland, gói `resolute`): Chrome đúng 6/6 lượt, không mất commit; Firefox đúng 4/4 lượt; ô chữ GTK3 đúng 2/2 lượt. Với macro `e` → `em`, `ko` → `không`, gõ `e ko a` ra `em không a` trong ô chữ GTK3 và Chrome. Ô bình luận TikTok thật trong Chrome trên Ubuntu đã được gõ thử khi test #176, đúng 23/23 từ; không được thử lại trên gói cuối.

Linux Mint 22.1 (Cinnamon X11, gói `noble`): Chromium đúng 4/4 lượt. CachyOS (KDE Plasma Wayland, gói Arch): Chromium đúng 6/6 lượt trên hai trang Draft.js và một ô textarea thường.

Gói 1.5.3 (PR #151, sau các sửa cuối) được test ngày 19/10/2026 bằng cách gửi phím vào VM, dùng Gõ Sen, Telex. Mỗi lượt gõ hai lần cùng một dòng: `duyeet`, space, 5 backspace, `yeejt` → `duyệt`; `toan`, space, 2 backspace, `nf` → `toàn`; `ban`, space, 1 backspace, `f` → `bàn`; rồi `tieengs vieetj xin chaof cacs banj, hoom nay trowif ddepj quas` → `tiếng việt xin chào các bạn, hôm nay trời đẹp quá`. Linux Mint 22.1 (Cinnamon X11, gói `noble`): GTK3 text box, GTK3 text box ép dùng XIM và xed đều đúng; xterm (XIM) đúng 5/10 lượt, các lượt còn lại lỗi vài từ khi phím dồn tới; bản 1.5.2 trên cùng máy lỗi 2/5 lượt nên lỗi này đã có trước 1.5.3. Ubuntu 26.04 (GNOME Wayland, gói `resolute`): GTK3 text box, gnome-text-editor và xterm đều đúng; Wine 11.20 Notepad đúng cả ba trường hợp backspace, nhưng 2/4 dòng có một hoặc hai từ lỗi (`chaà`, `tiêễn`), so với 4/4 dòng lỗi ở bản 1.5.2 trên cùng máy; các lỗi còn lại đến từ Wine.

Gói build từ nhánh `patch` sau #143 và từ PR #149 (PR #149 cùng code với 1.5.3, chỉ khác số version) được gõ thử trên ba máy ảo, kiểu Gõ Sen, Telex: Ubuntu 26.04 (GNOME Wayland, gói `resolute`), Linux Mint 22.1 (Cinnamon X11, gói `noble`) và CachyOS (KDE Plasma Wayland, gói Arch). Gõ `duyeet`, dấu cách, xoá còn `du` rồi `yeejt` ra `duyệt`; gõ `toan`, xoá một chữ, `nf` ra `toàn`; gõ `ban`, dấu cách, xoá một chữ, `f` ra `bàn`. Đã thử trong gnome-text-editor, xed, ô chữ GTK3 và GTK4, Ptyxis, gnome-terminal, Konsole, kitty, VS Code, Chrome và Firefox; giữ phím xoá rồi gõ tiếp vẫn đúng. Kiểu gõ VNI cũng được gõ thử với lỗi xoá qua dấu cách. LibreOffice và Firefox bản snap chưa được sửa lỗi xoá qua dấu cách, nhưng gõ không bị hỏng thêm.

Gói 1.5.2 (PR #130) được test trên máy ảo Ubuntu 26.04 (GNOME Wayland, gói `resolute`): ở trang Phím › Phím tắt, nút phím của cả năm chế độ hiện đủ khi cửa sổ nhỏ, khi phóng to, và khi thu nhỏ lại sau đó.

Gói 1.5.0 (PR #125) được gõ thử trên ba máy ảo: Linux Mint 22.1 (Cinnamon X11, gói `noble`), Ubuntu 26.04 (GNOME Wayland, gói `resolute`) và MX Linux 25 (Xfce X11, gói `trixie`, lần đầu dùng lõi Rust). Trên cả ba, gõ `tieengs vieetj` trong một ô chữ GTK3 ra `tiếng việt`; trên MX gõ thêm `nguwowif vieetj nam` ra `người việt nam`. Trên Ubuntu 26.04, chọn kiểu gõ Custom thì cửa sổ cài đặt hiện dòng nhắc, và "Kiểm tra máy tôi…" báo không có lỗi cài đặt. Gói 1.5.1 (PR #127) được gõ thử lại trên Mint và Ubuntu 26.04: bật "Tự viết hoa đầu câu", gõ `ban. tooi`, Enter, `xin` ra `Xin` ở dòng mới (gói 1.5.0 ra `xin`); gõ thường vẫn ra `tiếng việt`.

Người giữ dự án dùng Ngó Sen hằng ngày trên Fedora 44 (KDE Plasma Wayland). Kali đã được gõ thử ở các version trước. Fedora 43, Ubuntu 22.04, Debian 12 và openSUSE chỉ được build và qua test lúc build, chưa ai gõ thử trên máy thật.

Lõi gõ Rust nay có trong mọi gói trừ Debian 12, vẫn dùng lõi Go vì Rust có sẵn ở đó quá cũ.
