/*
 * Common SPI flash Interface
 *
 * Copyright (C) 2008 Atmel Corporation
 * Copyright (C) 2013 Jagannadha Sutradharudu Teki, Xilinx Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <dm.h>	/* Because we dereference struct udevice here */
#include <linux/types.h>

#ifndef CONFIG_SF_DEFAULT_SPEED
# define CONFIG_SF_DEFAULT_SPEED	1000000
#endif
#ifndef CONFIG_SF_DEFAULT_MODE
# define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3
#endif
#ifndef CONFIG_SF_DEFAULT_CS
# define CONFIG_SF_DEFAULT_CS		0
#endif
#ifndef CONFIG_SF_DEFAULT_BUS
# define CONFIG_SF_DEFAULT_BUS		0
#endif

struct spi_slave;

#define SPI_FLASH_PROTO_CMD_OFF	8
#define SPI_FLASH_PROTO_CMD_MASK	GENMASK(11, 8)
#define SPI_FLASH_PROTO_CMD_TO_PROTO(cmd) \
	(((cmd) << SPI_FLASH_PROTO_CMD_OFF) & SPI_FLASH_PROTO_CMD_MASK)
#define SPI_FLASH_PROTO_CMD_FROM_PROTO(proto) \
	((((u32)(proto)) & SPI_FLASH_PROTO_CMD_MASK) >> SPI_FLASH_PROTO_CMD_OFF)

#define SPI_FLASH_PROTO_ADR_OFF	4
#define SPI_FLASH_PROTO_ADR_MASK	GENMASK(7, 4)
#define SPI_FLASH_PROTO_ADR_TO_PROTO(adr) \
	(((adr) << SPI_FLASH_PROTO_ADR_OFF) & SPI_FLASH_PROTO_ADR_MASK)
#define SPI_FLASH_PROTO_ADR_FROM_PROTO(proto) \
	((((u32)(proto)) & SPI_FLASH_PROTO_ADR_MASK) >> SPI_FLASH_PROTO_ADR_OFF)

#define SPI_FLASH_PROTO_DAT_OFF	0
#define SPI_FLASH_PROTO_DAT_MASK	GENMASK(3, 0)
#define SPI_FLASH_PROTO_DAT_TO_PROTO(dat) \
	(((dat) << SPI_FLASH_PROTO_DAT_OFF) & SPI_FLASH_PROTO_DAT_MASK)
#define SPI_FLASH_PROTO_DAT_FROM_PROTO(proto) \
	((((u32)(proto)) & SPI_FLASH_PROTO_DAT_MASK) >> SPI_FLASH_PROTO_DAT_OFF)

#define SPI_FLASH_PROTO(cmd, adr, dat)	     \
	(SPI_FLASH_PROTO_CMD_TO_PROTO(cmd) | \
	 SPI_FLASH_PROTO_ADR_TO_PROTO(adr) | \
	 SPI_FLASH_PROTO_DAT_TO_PROTO(dat))

enum spi_flash_protocol {
	SPI_FLASH_PROTO_1_1_1 = SPI_FLASH_PROTO(1, 1, 1), /* SPI */
	SPI_FLASH_PROTO_1_1_2 = SPI_FLASH_PROTO(1, 1, 2), /* Dual Output */
	SPI_FLASH_PROTO_1_1_4 = SPI_FLASH_PROTO(1, 1, 4), /* Quad Output */
	SPI_FLASH_PROTO_1_2_2 = SPI_FLASH_PROTO(1, 2, 2), /* Dual IO */
	SPI_FLASH_PROTO_1_4_4 = SPI_FLASH_PROTO(1, 4, 4), /* Quad IO */
	SPI_FLASH_PROTO_2_2_2 = SPI_FLASH_PROTO(2, 2, 2), /* Dual Command */
	SPI_FLASH_PROTO_4_4_4 = SPI_FLASH_PROTO(4, 4, 4), /* Quad Command */
};

/**
 * struct spi_flash - SPI flash structure
 *
 * @spi:		SPI slave
 * @dev:		SPI flash device
 * @name:		Name of SPI flash
 * @dual_flash:		Indicates dual flash memories - dual stacked, parallel
 * @shift:		Flash shift useful in dual parallel
 * @flags:		Indication of spi flash flags
 * @size:		Total flash size
 * @page_size:		Write (page) size
 * @sector_size:	Sector size
 * @erase_size:		Erase size
 * @bank_read_cmd:	Bank read cmd
 * @bank_write_cmd:	Bank write cmd
 * @bank_curr:		Current flash bank
 * @erase_cmd:		Erase cmd 4K, 32K, 64K
 * @read_cmd:		Read cmd - Array Fast, Extn read and quad read.
 * @write_cmd:		Write cmd - page and quad program.
 * @dummy_byte:		Dummy cycles for read operation.
 * @reg_proto		SPI protocol to be used by &read_reg and &write_reg ops
 * @read_proto		SPI protocol to be used by &read ops
 * @write_proto		SPI protocol to be used by &write ops
 * @erase_proto		SPI protocol to be used by &erase ops
 * @addr_width		Number of address bytes (typically 3 or 4)
 * @memory_map:		Address of read-only SPI flash access
 * @flash_lock:		lock a region of the SPI Flash
 * @flash_unlock:	unlock a region of the SPI Flash
 * @flash_is_locked:	check if a region of the SPI Flash is completely locked
 * @read_reg:		Flash read_reg ops: Send cmd to read len bytes into buf
 * @write_reg:		Flash write_reg ops: Send cmd to write len bytes from
 *			buf
 * @read:		Flash read ops: Read len bytes at offset into buf
 *			Supported cmds: Fast Array Read
 * @write:		Flash write ops: Write len bytes from buf into offset
 *			Supported cmds: Page Program
 * @erase:		Flash erase ops: Erase len bytes from offset
 *			Supported cmds: Sector erase 4K, 32K, 64K
 * return 0 - Success, 1 - Failure
 */
struct spi_flash {
	struct spi_slave *spi;
#ifdef CONFIG_DM_SPI_FLASH
	struct udevice *dev;
#endif
	const char *name;
	u8 dual_flash;
	u8 shift;
	u16 flags;

	u32 size;
	u32 page_size;
	u32 sector_size;
	u32 erase_size;
	u32 addr_width;
#ifdef CONFIG_SPI_FLASH_BAR
	u8 bank_read_cmd;
	u8 bank_write_cmd;
	u8 bank_curr;
#endif
	u8 erase_cmd;
	u8 read_cmd;
	u8 write_cmd;
	u8 dummy_byte;

	enum spi_flash_protocol reg_proto;
	enum spi_flash_protocol read_proto;
	enum spi_flash_protocol write_proto;
	enum spi_flash_protocol erase_proto;

	void *memory_map;

	int (*flash_lock)(struct spi_flash *flash, u32 ofs, size_t len);
	int (*flash_unlock)(struct spi_flash *flash, u32 ofs, size_t len);
	int (*flash_is_locked)(struct spi_flash *flash, u32 ofs, size_t len);
#ifndef CONFIG_DM_SPI_FLASH
	/*
	 * These are not strictly needed for driver model, but keep them here
	 * while the transition is in progress.
	 *
	 * Normally each driver would provide its own operations, but for
	 * SPI flash most chips use the same algorithms. One approach is
	 * to create a 'common' SPI flash device which knows how to talk
	 * to most devices, and then allow other drivers to be used instead
	 * if required, perhaps with a way of scanning through the list to
	 * find the driver that matches the device.
	 */
	int (*read_reg)(struct spi_flash *flash, u8 opcode, size_t len,
			void *buf);
	int (*write_reg)(struct spi_flash *flash, u8 opcode, size_t len,
			 const void *buf);
	int (*read)(struct spi_flash *flash, u32 offset, size_t len, void *buf);
	int (*write)(struct spi_flash *flash, u32 offset, size_t len,
			const void *buf);
	int (*erase)(struct spi_flash *flash, u32 offset, size_t len);
#endif
};

struct dm_spi_flash_ops {
	int (*read_reg)(struct udevice *dev, u8 opcode, size_t len, void *buf);
	int (*write_reg)(struct udevice *dev, u8 opcode, size_t len,
			 const void *buf);
	int (*read)(struct udevice *dev, u32 offset, size_t len, void *buf);
	int (*write)(struct udevice *dev, u32 offset, size_t len,
		     const void *buf);
	int (*erase)(struct udevice *dev, u32 offset, size_t len);
};

/* Access the serial operations for a device */
#define sf_get_ops(dev) ((struct dm_spi_flash_ops *)(dev)->driver->ops)

#ifdef CONFIG_DM_SPI_FLASH
/**
 * spi_flash_read_reg_dm() - Send register command and read data result
 *
 * @dev:	SPI flash device
 * @opcode:	Register command op code
 * @len:	Number of bytes to read as register command output
 * @buf:	Buffer to fill with the register command output
 * @return 0 if OK, -ve on error
 */
int spi_flash_read_reg_dm(struct udevice *dev, u8 opcode, size_t len,
			  void *buf);

/**
 * spi_flash_write_reg_dm() - Send register command with data
 *
 * @dev:	SPI flash device
 * @opcode:	Register command op code
 * @len:	Number of bytes to write as register command input
 * @buf:	Buffer filled with the register command input
 * @return 0 if OK, -ve on error
 */
int spi_flash_write_reg_dm(struct udevice *dev, u8 opcode, size_t len,
			   const void *buf);

/**
 * spi_flash_read_dm() - Read data from SPI flash
 *
 * @dev:	SPI flash device
 * @offset:	Offset into device in bytes to read from
 * @len:	Number of bytes to read
 * @buf:	Buffer to put the data that is read
 * @return 0 if OK, -ve on error
 */
int spi_flash_read_dm(struct udevice *dev, u32 offset, size_t len, void *buf);

/**
 * spi_flash_write_dm() - Write data to SPI flash
 *
 * @dev:	SPI flash device
 * @offset:	Offset into device in bytes to write to
 * @len:	Number of bytes to write
 * @buf:	Buffer containing bytes to write
 * @return 0 if OK, -ve on error
 */
int spi_flash_write_dm(struct udevice *dev, u32 offset, size_t len,
		       const void *buf);

/**
 * spi_flash_erase_dm() - Erase blocks of the SPI flash
 *
 * Note that @len must be a muiltiple of the flash sector size.
 *
 * @dev:	SPI flash device
 * @offset:	Offset into device in bytes to start erasing
 * @len:	Number of bytes to erase
 * @return 0 if OK, -ve on error
 */
int spi_flash_erase_dm(struct udevice *dev, u32 offset, size_t len);

int spi_flash_probe_bus_cs(unsigned int busnum, unsigned int cs,
			   unsigned int max_hz, unsigned int spi_mode,
			   struct udevice **devp);

/* Compatibility function - this is the old U-Boot API */
struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int spi_mode);

/* Compatibility function - this is the old U-Boot API */
void spi_flash_free(struct spi_flash *flash);

static inline int spi_flash_read_reg(struct spi_flash *flash, u8 opcode,
				     size_t len, void *buf)
{
	return spi_flash_read_reg_dm(flash->dev, opcode, len, buf);
}

static inline int spi_flash_write_reg(struct spi_flash *flash, u8 opcode,
				      size_t len, const void *buf)
{
	return spi_flash_write_reg_dm(flash->dev, opcode, len, buf);
}

static inline int spi_flash_read(struct spi_flash *flash, u32 offset,
				 size_t len, void *buf)
{
	return spi_flash_read_dm(flash->dev, offset, len, buf);
}

static inline int spi_flash_write(struct spi_flash *flash, u32 offset,
				  size_t len, const void *buf)
{
	return spi_flash_write_dm(flash->dev, offset, len, buf);
}

static inline int spi_flash_erase(struct spi_flash *flash, u32 offset,
				  size_t len)
{
	return spi_flash_erase_dm(flash->dev, offset, len);
}

struct sandbox_state;

int sandbox_sf_bind_emul(struct sandbox_state *state, int busnum, int cs,
			 struct udevice *bus, int of_offset, const char *spec);

void sandbox_sf_unbind_emul(struct sandbox_state *state, int busnum, int cs);

#else
struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode);

/**
 * Set up a new SPI flash from an fdt node
 *
 * @param blob		Device tree blob
 * @param slave_node	Pointer to this SPI slave node in the device tree
 * @param spi_node	Cached pointer to the SPI interface this node belongs
 *			to
 * @return 0 if ok, -1 on error
 */
struct spi_flash *spi_flash_probe_fdt(const void *blob, int slave_node,
				      int spi_node);

void spi_flash_free(struct spi_flash *flash);

static inline int spi_flash_read_reg(struct spi_flash *flash, u8 opcode,
				     size_t len, void *buf)
{
	return flash->read_reg(flash, opcode, len, buf);
}

static inline int spi_flash_write_reg(struct spi_flash *flash, u8 opcode,
				      size_t len, const void *buf)
{
	return flash->write_reg(flash, opcode, len, buf);
}

static inline int spi_flash_read(struct spi_flash *flash, u32 offset,
		size_t len, void *buf)
{
	return flash->read(flash, offset, len, buf);
}

static inline int spi_flash_write(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf)
{
	return flash->write(flash, offset, len, buf);
}

static inline int spi_flash_erase(struct spi_flash *flash, u32 offset,
		size_t len)
{
	return flash->erase(flash, offset, len);
}
#endif

static inline int spi_flash_protect(struct spi_flash *flash, u32 ofs, u32 len,
					bool prot)
{
	if (!flash->flash_lock || !flash->flash_unlock)
		return -EOPNOTSUPP;

	if (prot)
		return flash->flash_lock(flash, ofs, len);
	else
		return flash->flash_unlock(flash, ofs, len);
}

void spi_boot(void) __noreturn;
void spi_spl_load_image(uint32_t offs, unsigned int size, void *vdst);

#endif /* _SPI_FLASH_H_ */
