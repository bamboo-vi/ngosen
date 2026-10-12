Bản dựng tự động từ nhánh `dev`, commit @SHA@. Đây là bản để test trước khi phát hành, có thể còn lỗi.
Những gì mới so với bản chính thức nằm ở mục "Chưa phát hành" trong
[CHANGELOG](https://github.com/ngosen/ngosen/blob/dev/CHANGELOG.md).

Cài hoặc cập nhật bản thử:

```
curl -fsSL https://raw.githubusercontent.com/ngosen/ngosen/dev/install.sh | NGOSEN_RELEASE_URL=https://github.com/ngosen/ngosen/releases/download/dev-build bash
```

Số phiên bản có thêm ngày giờ và mã commit, ví dụ `1.2.2-1.202610121530.git8cb4b89`. Khi bản chính thức
tiếp theo ra, chạy lệnh cài thường trong README là máy lên bản đó.

Gặp lỗi thì báo ở [Issues](https://github.com/ngosen/ngosen/issues), ghi kèm số phiên bản trên.
