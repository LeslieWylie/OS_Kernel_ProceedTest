/**
 * vfs.c - 虚拟文件系统接口
 */

#include <stdint.h>
#include <stddef.h>
#include "../../include/types.h"

// 文件系统类型常量
#define FS_TYPE_EXT2 1
#define FS_TYPE_FAT32 2
#define FS_TYPE_ISO9660 3
#define FS_TYPE_PROC 4

// 文件系统状态
#define FS_MOUNTED 1
#define FS_UNMOUNTED 0

// 文件打开标志
#define O_RDONLY 0x0001
#define O_WRONLY 0x0002
#define O_RDWR 0x0003
#define O_CREAT 0x0100
#define O_APPEND 0x0200
#define O_TRUNC 0x0400

// 文件查找标志
#define FS_LOOKUP_NORMAL 0
#define FS_LOOKUP_CREATE 1
#define FS_LOOKUP_DELETE 2

// 文件系统前向声明
struct file_system;
struct dentry;
struct inode;
struct file;

// 文件操作
typedef struct file_operations
{
    int (*open)(struct inode *, struct file *);
    ssize_t (*read)(struct file *, char *, size_t, off_t *);
    ssize_t (*write)(struct file *, const char *, size_t, off_t *);
    int (*close)(struct file *);
    off_t (*lseek)(struct file *, off_t, int);
    int (*ioctl)(struct file *, unsigned int, unsigned long);
    int (*fsync)(struct file *);
} file_operations_t;

// inode操作
typedef struct inode_operations
{
    struct dentry *(*lookup)(struct inode *, struct dentry *, int);
    int (*create)(struct inode *, struct dentry *, int);
    int (*unlink)(struct inode *, struct dentry *);
    int (*mkdir)(struct inode *, struct dentry *, int);
    int (*rmdir)(struct inode *, struct dentry *);
} inode_operations_t;

// 超级块操作
typedef struct super_operations
{
    int (*sync_fs)(struct file_system *);
    int (*statfs)(struct file_system *, struct statfs *);
} super_operations_t;

// 文件系统类型
typedef struct file_system_type
{
    const char *name;
    int fs_flags;
    struct file_system *(*mount)(const char *, const char *, const char *, void *);
    int (*unmount)(struct file_system *);
    struct file_system_type *next;
} file_system_type_t;

// 文件系统实例(挂载点)
typedef struct file_system
{
    const char *dev_name;
    const char *mount_point;
    struct file_system_type *fs_type;
    struct super_operations *s_op;
    void *private_data;
    struct dentry *root;
    int flags;
} file_system_t;

// 目录项
typedef struct dentry
{
    char *name;
    struct inode *inode;
    struct dentry *parent;
    struct dentry *next;
    struct dentry *child;
} dentry_t;

// 索引节点
typedef struct inode
{
    uint64_t i_ino;
    uint32_t i_mode;
    uint32_t i_uid;
    uint32_t i_gid;
    uint32_t i_nlink;
    uint64_t i_size;
    uint64_t i_atime;
    uint64_t i_mtime;
    uint64_t i_ctime;
    struct inode_operations *i_op;
    struct file_operations *f_op;
    struct file_system *fs;
    void *private_data;
} inode_t;

// 打开的文件
typedef struct file
{
    struct dentry *dentry;
    struct inode *inode;
    uint32_t f_mode;
    uint32_t f_flags;
    uint64_t f_pos;
    struct file_operations *f_op;
    void *private_data;
} file_t;

// 已注册的文件系统类型
static file_system_type_t *registered_fs_types = NULL;

// 已挂载的文件系统
static file_system_t *mounted_fs_list = NULL;

// 打开的文件表
static file_t *open_files[256] = {NULL};

/**
 * 注册文件系统类型
 *
 * @param fs_type 要注册的文件系统类型
 * @return 成功返回0，失败返回错误码
 */
int register_filesystem(file_system_type_t *fs_type)
{
    if (!fs_type || !fs_type->name)
        return -E_INVAL;

    // 检查是否已经注册
    file_system_type_t *p = registered_fs_types;
    while (p)
    {
        if (strcmp(p->name, fs_type->name) == 0)
            return -E_EXIST;
        p = p->next;
    }

    // 添加到链表头部
    fs_type->next = registered_fs_types;
    registered_fs_types = fs_type;

    return 0;
}

/**
 * 查找文件系统类型
 *
 * @param name 文件系统类型名称
 * @return 找到返回文件系统类型指针，否则返回NULL
 */
file_system_type_t *find_filesystem(const char *name)
{
    file_system_type_t *p = registered_fs_types;
    while (p)
    {
        if (strcmp(p->name, name) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}

/**
 * 挂载文件系统
 *
 * @param fs_type 文件系统类型
 * @param dev 设备名
 * @param mount_point 挂载点路径
 * @param flags 挂载标志
 * @param data 特定文件系统数据
 * @return 成功返回0，失败返回错误码
 */
int mount_fs(const char *fs_type_name, const char *dev, const char *mount_point,
             int flags, void *data)
{
    // 查找文件系统类型
    file_system_type_t *fs_type = find_filesystem(fs_type_name);
    if (!fs_type)
        return -E_NOTFOUND;

    // 调用文件系统特定的挂载函数
    file_system_t *fs = fs_type->mount(fs_type_name, dev, mount_point, data);
    if (!fs)
        return -E_IO;

    // 设置标志和类型
    fs->flags = FS_MOUNTED | flags;
    fs->fs_type = fs_type;

    // 添加到挂载点链表
    // 简化处理，没有检查挂载点是否已经被挂载

    return 0;
}

/**
 * 初始化虚拟文件系统
 */
void vfs_init(void)
{
    // 初始化各种结构
    registered_fs_types = NULL;
    mounted_fs_list = NULL;

    // 清空打开文件表
    for (int i = 0; i < 256; i++)
    {
        open_files[i] = NULL;
    }

    // 初始化根文件系统
    // 通常会挂载一个初始ramdisk
}