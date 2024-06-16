# 汇编函数库说明
## 借助BIOS中断实现的函数
- position_cursor: 重置光标位置（默认第0页）
- clear_screen: 清空全屏，且将光标设置到(0,0)处
- print: 在界面显示字符串
- read_disk_BIOS: 使用BIOS的0x13中断读取磁盘扇区
- read_disk_LBA: 采用IDE/ATA PIO方式读取磁盘扇区

