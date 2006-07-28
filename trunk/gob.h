#ifndef GOB_H
#define GOB_H

#define GOB_PATH_LENGTH 128

struct gob_header
{
	char			_magic[4];
	char			_version;
	int				_itemOffset;
	int				_numItems;
};

struct gob_item
{
	int				_offset;
	int				_length;
	char			_name[GOB_PATH_LENGTH];
};

#endif // GOB_H
