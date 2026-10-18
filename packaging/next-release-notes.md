Build tự động từ nhánh `next`, commit @SHA@. Đây là version để test trước khi phát hành, có thể còn lỗi. Những gì mới so với version chính thức nằm ở mục "Chưa phát hành" trong [CHANGELOG](https://github.com/ngosen/ngosen/blob/next/CHANGELOG.md).

Cài hoặc cập nhật version thử:

```
curl -fsSL https://raw.githubusercontent.com/ngosen/ngosen/next/install.sh | NGOSEN_RELEASE_URL=https://github.com/ngosen/ngosen/releases/download/next-build bash
```

Số version có thêm ngày giờ và mã commit, ví dụ `1.2.2-1.202610121530.git8cb4b89`. Khi version chính thức tiếp theo ra, chạy lệnh cài thường trong README là máy lên version đó.

Gặp lỗi thì báo ở [Issues](https://github.com/ngosen/ngosen/issues), ghi kèm số version trên.
