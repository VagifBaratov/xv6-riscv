#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define EXT2_SUPER_MAGIC 0xEF53

typedef uint32_t __u32;
typedef uint16_t __u16;
typedef uint8_t __u8;

struct ext2_super_block {
    __u32 s_inodes_count;
    __u32 s_blocks_count;
    __u32 s_r_blocks_count;
    __u32 s_free_blocks_count;
    __u32 s_free_inodes_count;
    __u32 s_first_data_block;
    __u32 s_log_block_size;
    __u32 s_log_frag_size;
    __u32 s_blocks_per_group;
    __u32 s_frags_per_group;
    __u32 s_inodes_per_group;
    __u32 s_mtime;
    __u32 s_wtime;
    __u16 s_mnt_count;
    __u16 s_max_mnt_count;
    __u16 s_magic;
    __u16 s_state;
    __u16 s_errors;
    __u16 s_minor_rev_level;
    __u32 s_lastcheck;
    __u32 s_checkinterval;
    __u32 s_creator_os;
    __u32 s_rev_level;
    __u16 s_def_resuid;
    __u16 s_def_resgid;
    __u32 s_first_ino;
    __u16 s_inode_size;
    __u16 s_block_group_nr;
    __u32 s_feature_compat;
    __u32 s_feature_incompat;
    __u32 s_feature_ro_compat;
    __u8 s_uuid[16];
    __u8 s_volume_name[16];
    __u8 s_last_mounted[64];
    __u32 s_algorithm_usage_bitmap;
    __u8 s_prealloc_blocks;
    __u8 s_prealloc_dir_blocks;
    __u16 s_padding1;
    __u8 s_journal_uuid[16];
    __u32 s_journal_inum;
    __u32 s_journal_dev;
    __u32 s_last_orphan;
    __u32 s_hash_seed[4];
    __u8 s_def_hash_version;
    __u8 s_jnl_backup_type;
    __u16 s_desc_size;
    __u32 s_default_mount_opts;
    __u32 s_first_meta_bg;
    __u32 s_mkfs_time;
    __u32 s_jnl_blocks[17];
};

struct ext2_group_desc {
    __u32 bg_block_bitmap;
    __u32 bg_inode_bitmap;
    __u32 bg_inode_table;
    __u16 bg_free_blocks_count;
    __u16 bg_free_inodes_count;
    __u16 bg_used_dirs_count;
    __u16 bg_pad;
    __u32 bg_reserved[3];
};

struct ext2_inode {
    __u16 i_mode;
    __u16 i_uid;
    __u32 i_size;
    __u32 i_atime;
    __u32 i_ctime;
    __u32 i_mtime;
    __u32 i_dtime;
    __u16 i_gid;
    __u16 i_links_count;
    __u32 i_blocks;
    __u32 i_flags;
    __u32 i_osd1;
    __u32 i_block[15];
    __u32 i_generation;
    __u32 i_file_acl;
    __u32 i_dir_acl;
    __u32 i_faddr;
    __u8  i_osd2[12];
};

uint32_t read32(uint32_t value) {
    if (le32toh(0x01020304) == 0x01020304) {
        return value; // little-endian
    } else {
        return ((value >> 24) & 0xff) | ((value >> 8) & 0xff00) |
               ((value << 8) & 0xff0000) | ((value << 24) & 0xff000000);
    }
}

uint16_t read16(uint16_t value) {
    if (le16toh(0x0102) == 0x0102) {
        return value; // little-endian
    } else {
        return ((value >> 8) & 0xff) | ((value << 8) & 0xff00);
    }
}

void read_superblock(FILE *file, struct ext2_super_block *sb, uint32_t block_size) {
    fseek(file, 1024, SEEK_SET);
    fread(sb, sizeof(struct ext2_super_block), 1, file);
    
    sb->s_inodes_count = read32(sb->s_inodes_count);
    sb->s_blocks_count = read32(sb->s_blocks_count);
    sb->s_first_data_block = read32(sb->s_first_data_block);
    sb->s_log_block_size = read32(sb->s_log_block_size);
    sb->s_blocks_per_group = read32(sb->s_blocks_per_group);
    sb->s_inodes_per_group = read32(sb->s_inodes_per_group);
    sb->s_magic = read16(sb->s_magic);
    sb->s_inode_size = read16(sb->s_inode_size);
}

void read_group_desc(FILE *file, struct ext2_group_desc *gd, uint32_t block_size, uint32_t group) {
    uint32_t gd_block = (block_size == 1024) ? 2 : 1;
    fseek(file, gd_block * block_size + group * sizeof(struct ext2_group_desc), SEEK_SET);
    fread(gd, sizeof(struct ext2_group_desc), 1, file);
    
    gd->bg_block_bitmap = read32(gd->bg_block_bitmap);
    gd->bg_inode_bitmap = read32(gd->bg_inode_bitmap);
    gd->bg_inode_table = read32(gd->bg_inode_table);
}

void read_inode(FILE *file, struct ext2_inode *inode, uint32_t block_size, 
                uint32_t inode_table_block, uint32_t inode_num, uint16_t inode_size) {
    uint32_t inode_index = (inode_num - 1) % inode_size;
    uint32_t inode_offset = inode_index * inode_size;
    uint32_t inode_block = inode_table_block + (inode_offset / block_size);
    uint32_t inode_block_offset = inode_offset % block_size;
    
    fseek(file, inode_block * block_size + inode_block_offset, SEEK_SET);
    fread(inode, sizeof(struct ext2_inode), 1, file);
    
    inode->i_mode = read16(inode->i_mode);
    inode->i_uid = read16(inode->i_uid);
    inode->i_size = read32(inode->i_size);
    inode->i_blocks = read32(inode->i_blocks);
    inode->i_gid = read16(inode->i_gid);
    inode->i_links_count = read16(inode->i_links_count);
    
    for (int i = 0; i < 15; i++) {
        inode->i_block[i] = read32(inode->i_block[i]);
    }
}

void read_data_block(FILE *file, uint32_t block_num, uint32_t block_size, void *buffer) {
    fseek(file, block_num * block_size, SEEK_SET);
    fread(buffer, block_size, 1, file);
}

void read_indirect_blocks(FILE *file, uint32_t block_num, uint32_t block_size, 
                          uint32_t *blocks, uint32_t *count, uint32_t max_blocks) {
    uint32_t *block_pointers = malloc(block_size);
    read_data_block(file, block_num, block_size, block_pointers);
    
    uint32_t pointers_per_block = block_size / sizeof(uint32_t);
    
    for (uint32_t i = 0; i < pointers_per_block && *count < max_blocks; i++) {
        blocks[(*count)++] = read32(block_pointers[i]);
    }
    
    free(block_pointers);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <image_file> <inode_number>\n", argv[0]);
        return 1;
    }
    
    const char *image_file = argv[1];
    uint32_t inode_num = atoi(argv[2]);
    
    FILE *file = fopen(image_file, "rb");
    if (!file) {
        perror("Failed to open image file");
        return 1;
    }
    
    struct ext2_super_block sb;
    read_superblock(file, &sb, 1024);
    
    if (sb.s_magic != EXT2_SUPER_MAGIC) {
        fprintf(stderr, "Not an ext2 filesystem\n");
        fclose(file);
        return 1;
    }
    
    uint32_t block_size = 1024 << sb.s_log_block_size;
    uint32_t group = (inode_num - 1) / sb.s_inodes_per_group;
    
    struct ext2_group_desc gd;
    read_group_desc(file, &gd, block_size, group);
    
    struct ext2_inode inode;
    read_inode(file, &inode, block_size, gd.bg_inode_table, inode_num, sb.s_inode_size);
    
    uint64_t file_size = inode.i_size;
    if (inode.i_dir_acl) {
        file_size |= ((uint64_t)inode.i_dir_acl << 32);
    }
    
    uint32_t max_blocks = inode.i_blocks / (2 << sb.s_log_block_size);
    uint32_t *blocks = malloc(max_blocks * sizeof(uint32_t));
    uint32_t block_count = 0;
    
    for (int i = 0; i < 12 && block_count < max_blocks; i++) {
        if (inode.i_block[i]) {
            blocks[block_count++] = inode.i_block[i];
        }
    }
    
    if (inode.i_block[12] && block_count < max_blocks) {
        read_indirect_blocks(file, inode.i_block[12], block_size, blocks, &block_count, max_blocks);
    }
    
    if (inode.i_block[13] && block_count < max_blocks) {
        uint32_t *first_level = malloc(block_size);
        read_data_block(file, inode.i_block[13], block_size, first_level);
        
        uint32_t pointers_per_block = block_size / sizeof(uint32_t);
        
        for (uint32_t i = 0; i < pointers_per_block && block_count < max_blocks; i++) {
            uint32_t second_level_block = read32(first_level[i]);
            if (second_level_block) {
                read_indirect_blocks(file, second_level_block, block_size, blocks, &block_count, max_blocks);
            }
        }
        
        free(first_level);
    }
    
    uint8_t *buffer = malloc(block_size);
    uint64_t remaining = file_size;
    
    for (uint32_t i = 0; i < block_count && remaining > 0; i++) {
        uint32_t to_read = (remaining > block_size) ? block_size : remaining;
        read_data_block(file, blocks[i], block_size, buffer);
        fwrite(buffer, 1, to_read, stdout);
        remaining -= to_read;
    }
    
    free(buffer);
    free(blocks);
    fclose(file);
    
    return 0;
}