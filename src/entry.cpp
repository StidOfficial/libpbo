#include "entry.hpp"

namespace PBO
{
	Entry::Entry()
		: m_packing_method(PBO::PackingMethod::Null), m_original_size(0), m_reserved(0),
			m_timestamp(0), m_data_size(0), m_data_offset(-1)
	{
	}

	void Entry::set_file_path(std::filesystem::path file_path)
	{
		m_file_path = file_path;

		set_data_size(static_cast<uint32_t>(std::filesystem::file_size(m_file_path)));
		std::filesystem::file_time_type last_write_time = std::filesystem::last_write_time(m_file_path);
		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(last_write_time
			- std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

		std::time_t entry_time = std::chrono::system_clock::to_time_t(sctp);
		set_timestamp(static_cast<uint32_t>(entry_time));
	}

	std::filesystem::path Entry::get_file_path()
	{
		return m_file_path;
	}

	void Entry::set_path(std::filesystem::path path)
	{
		if(path.string().length() > ENTRY_PATH_LEN)
			throw std::length_error("Invalid path length");

		m_path = path;
	}

	std::filesystem::path Entry::get_path()
	{
		return m_path;
	}

	void Entry::set_packing_method(uint32_t packing_method)
	{
		m_packing_method = packing_method;
	}

	uint32_t Entry::get_packing_method()
	{
		return m_packing_method;
	}

	void Entry::set_original_size(uint32_t original_size)
	{
		m_original_size = original_size;
	}

	uint32_t Entry::get_original_size()
	{
		return m_original_size;
	}

	void Entry::set_reserved(uint32_t reserved)
	{
		m_reserved = reserved;
	}

	uint32_t Entry::get_reserved()
	{
		return m_reserved;
	}

	void Entry::set_timestamp(uint32_t timestamp)
	{
		m_timestamp = timestamp;
	}

	uint32_t Entry::get_timestamp()
	{
		return m_timestamp;
	}

	void Entry::set_data_size(uint32_t data_size)
	{
		m_data_size = data_size;
	}

	int Entry::get_data_size()
	{
		return m_data_size;
	}

	void Entry::set_data_offset(std::streampos data_offset)
	{
		m_data_offset = data_offset;
	}

	std::streampos Entry::get_data_offset()
	{
		return m_data_offset;
	}

	Properties &Entry::get_properties()
	{
		return m_properties;
	}

	bool Entry::is_version_entry()
	{
		return get_packing_method() == PBO::PackingMethod::Version;
	}

	bool Entry::is_file_entry()
	{
		return get_packing_method() == PBO::PackingMethod::Uncompressed ||
			get_packing_method() == PBO::PackingMethod::Compressed ||
			get_packing_method() == PBO::PackingMethod::Encrypted;
	}

	bool Entry::is_zero_entry()
	{
		return m_path.string().length() == 0 &&
			m_packing_method == 0 &&
			m_original_size == 0 &&
			m_reserved == 0 &&
			m_timestamp == 0 &&
			m_data_size == 0;
	}

	Entry::~Entry()
	{
	}
}
