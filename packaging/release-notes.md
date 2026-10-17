Version 1.4.3 sửa một lỗi: bỏ chọn "Từ điển tuỳ chỉnh" thì Ngó Sen quay về từ điển gốc. Trước đây, trên distro dùng fcitx5 cũ hơn 5.1.13 (Ubuntu 22.04, 24.04, Linux Mint 22, Debian 12, 13, MX Linux 25), từ thêm vào từ điển tuỳ chỉnh vẫn được giữ dấu sau khi bỏ chọn. Chi tiết ở [CHANGELOG.md](https://github.com/ngosen/ngosen/blob/main/CHANGELOG.md).

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

Version 1.4.3 được gõ thử bằng gói `…_noble_amd64.deb` build từ PR #111, trên máy ảo Linux Mint 22.1 (Cinnamon X11, fcitx5 5.1.7). Từ điển tuỳ chỉnh có thêm `kạ`, mỗi lần đổi ô thì khởi động lại fcitx5: bật "Từ điển tuỳ chỉnh" thì `kaj` ra `kạ` 2/2 lần, bỏ chọn thì ra `kaj` 2/2 lần. Cùng các bước đó với 1.4.2 thì cả hai lần đều ra `kạ`.

Người giữ dự án dùng Ngó Sen hằng ngày trên Fedora 44 (KDE Plasma Wayland). Ubuntu 26.04, Debian 13, Arch và Kali đã được gõ thử ở các version trước. Fedora 43, Ubuntu 22.04, Debian 12 và openSUSE chỉ được build và qua test lúc build, chưa ai gõ thử trên máy thật.

Lõi gõ Rust có trong gói Fedora, Arch, openSUSE và Ubuntu 24.04 trở lên. Debian 12, 13 và Ubuntu 22.04 dùng lõi Go, vì Rust có sẵn ở đó quá cũ. Phần sửa ở version này nằm ngoài lõi gõ nên có trong cả hai.
