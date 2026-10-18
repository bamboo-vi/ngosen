Version 1.5.0 thêm nút "Kiểm tra máy tôi…" ở trang Giới thiệu của cửa sổ cài đặt: xem fcitx5 có chạy không, Ngó Sen có trong danh sách bộ gõ không, các biến môi trường bộ gõ có đúng không, và test một app xem gõ có đúng không. Mỗi lỗi kèm cách sửa, và nút "Báo cáo lỗi…" mở trang báo lỗi đã điền sẵn kết quả. Gói Debian 13 và Ubuntu 22.04 chuyển sang lõi gõ Rust. Version này cũng sửa hai lỗi ở cửa sổ cài đặt: chọn kiểu gõ Custom khi chưa có phím nào thì nay có dòng nhắc, và cảnh báo trùng phím tắt không còn đè lên tên chế độ. Chi tiết ở [CHANGELOG.md](https://github.com/ngosen/ngosen/blob/main/CHANGELOG.md).

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

TODO

Lõi gõ Rust nay có trong mọi gói trừ Debian 12, vẫn dùng lõi Go vì Rust có sẵn ở đó quá cũ.
