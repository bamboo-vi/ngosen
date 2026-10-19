Version 1.5.3 sửa lỗi Gõ Sen khi xoá lùi rồi gõ tiếp. Xoá qua dấu cách vào lại từ vừa gõ rồi gõ tiếp thì Ngó Sen sửa đúng từ đó (`duyeet`, dấu cách, xoá còn `du`, gõ `yeejt` ra `duyệt`, trước đây ra `duyeejt`), cả kiểu gõ Telex lẫn VNI. Trong gnome-text-editor trên GNOME, sửa chữ giữa từ không còn mất chữ (`toan`, xoá một chữ, `nf` ra `toàn`, trước đây ra `toòn`). Nhật ký gõ ghi đúng app khi chuyển cửa sổ, và nút "Mặc định" trong cửa sổ cài đặt sáng đúng lúc. Đây cũng là version đầu tiên được đưa lên kho cài đặt cho Fedora (COPR), Debian/Ubuntu, Arch và openSUSE; cách bật kho sẽ ghi trong README khi các kho đã chạy ổn. Chi tiết ở [CHANGELOG.md](https://github.com/ngosen/ngosen/blob/main/CHANGELOG.md).

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
- Ubuntu 26.04, 24.04, 22.04: `…_resolute_amd64.deb`, `…_noble_amd64.deb`, `…_jammy_amd64.deb`
- Debian 13, 12: `…_trixie_amd64.deb`, `…_bookworm_amd64.deb`
- Kali Rolling: `…_forky_amd64.deb`
- Máy ARM 64-bit: file cùng tên với đuôi `_arm64.deb`, hoặc `.aarch64.rpm` cho Fedora

Cài gói bằng tay trên Ubuntu 26.04 thì bật thêm [extension sửa lỗi gõ](https://github.com/ngosen/ngosen#ubuntu-2604-extension-sửa-lỗi-gõ).

Đối chiếu file tải về với `SHA256SUMS`: `sha256sum -c SHA256SUMS --ignore-missing`.

## Đã test những gì

Gói 1.5.3 (PR #151) được gõ thử trên Ubuntu 26.04 (GNOME Wayland, gói `resolute`, gnome-text-editor) và Linux Mint 22.1 (Cinnamon X11, gói `noble`, ô chữ GTK3 và xed): các ca xoá lùi ở dưới đều đúng, và `tieengs vieetj`, `xin chaof cacs banj, hoom nay trowif ddepj quas` ra `tiếng việt`, `xin chào các bạn, hôm nay trời đẹp quá`.

Gói build từ nhánh `patch` sau #143 và từ PR #149 (PR #149 cùng code với 1.5.3, chỉ khác số version) được gõ thử trên ba máy ảo, kiểu Gõ Sen, Telex: Ubuntu 26.04 (GNOME Wayland, gói `resolute`), Linux Mint 22.1 (Cinnamon X11, gói `noble`) và CachyOS (KDE Plasma Wayland, gói Arch). Gõ `duyeet`, dấu cách, xoá còn `du` rồi `yeejt` ra `duyệt`; gõ `toan`, xoá một chữ, `nf` ra `toàn`; gõ `ban`, dấu cách, xoá một chữ, `f` ra `bàn`. Đã thử trong gnome-text-editor, xed, ô chữ GTK3 và GTK4, Ptyxis, gnome-terminal, Konsole, kitty, VS Code, Chrome và Firefox; giữ phím xoá rồi gõ tiếp vẫn đúng. Kiểu gõ VNI cũng được gõ thử với lỗi xoá qua dấu cách. LibreOffice và Firefox bản snap chưa được sửa lỗi xoá qua dấu cách, nhưng gõ không bị hỏng thêm.

Gói 1.5.2 (PR #130) được test trên máy ảo Ubuntu 26.04 (GNOME Wayland, gói `resolute`): ở trang Phím › Phím tắt, nút phím của cả năm chế độ hiện đủ khi cửa sổ nhỏ, khi phóng to, và khi thu nhỏ lại sau đó.

Gói 1.5.0 (PR #125) được gõ thử trên ba máy ảo: Linux Mint 22.1 (Cinnamon X11, gói `noble`), Ubuntu 26.04 (GNOME Wayland, gói `resolute`) và MX Linux 25 (Xfce X11, gói `trixie`, lần đầu dùng lõi Rust). Trên cả ba, gõ `tieengs vieetj` trong một ô chữ GTK3 ra `tiếng việt`; trên MX gõ thêm `nguwowif vieetj nam` ra `người việt nam`. Trên Ubuntu 26.04, chọn kiểu gõ Custom thì cửa sổ cài đặt hiện dòng nhắc, và "Kiểm tra máy tôi…" báo không có lỗi cài đặt. Gói 1.5.1 (PR #127) được gõ thử lại trên Mint và Ubuntu 26.04: bật "Tự viết hoa đầu câu", gõ `ban. tooi`, Enter, `xin` ra `Xin` ở dòng mới (gói 1.5.0 ra `xin`); gõ thường vẫn ra `tiếng việt`.

Người giữ dự án dùng Ngó Sen hằng ngày trên Fedora 44 (KDE Plasma Wayland). Kali đã được gõ thử ở các version trước. Fedora 43, Ubuntu 22.04, Debian 12 và openSUSE chỉ được build và qua test lúc build, chưa ai gõ thử trên máy thật.

Lõi gõ Rust nay có trong mọi gói trừ Debian 12, vẫn dùng lõi Go vì Rust có sẵn ở đó quá cũ.
