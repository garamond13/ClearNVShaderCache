#include <Shlobj.h>

// STD
#include <filesystem>
#include <iostream>

static constexpr int VERSION[3]{ 1, 0, 0 };

struct Human_readable
{
	std::uintmax_t size{};
private:
	friend std::ostream& operator<<(std::ostream& os, Human_readable hr)
	{
		int o{};
		auto mantissa{ static_cast<double>(hr.size) };
		for (; mantissa >= 1024.0; mantissa /= 1024.0, ++o);
		os << std::ceil(mantissa * 10.0) / 10.0 << "BKMGTPE"[o];
		return o ? os << "B (" << hr.size << ')' : os;
	}
};

static void remove(std::filesystem::path& path)
{
	uintmax_t size_before{};
	uintmax_t size_after{};
	if (std::filesystem::exists(path)) {

		// Get size before.
		for (auto& tmp : std::filesystem::recursive_directory_iterator(path))
			size_before += std::filesystem::file_size(tmp);

		// Delete everything inside.
		for (auto& tmp : std::filesystem::directory_iterator(path)) {
			try {
				std::filesystem::remove_all(tmp);
			}
			catch (const std::filesystem::filesystem_error& e) {
				std::cerr << e.what() << '\n';
				continue;
			}
		}

		// Get size after.
		for (auto& tmp : std::filesystem::recursive_directory_iterator(path))
			size_after += std::filesystem::file_size(tmp);

		std::cout << path << " " << Human_readable(size_before - size_after) << " removed\n";
	}
	else
		std::cerr << path << " does not exists.\n";
}

int main()
{
	std::cout << "ClearNVShaderCache version " << VERSION[0] << '.' << VERSION[1] << '.' << VERSION[2] << '\n';
	PWSTR known_folder_path;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, nullptr, &known_folder_path);
	std::filesystem::path path{ std::move(known_folder_path) };

	// %LOCALAPPDATA%\D3DSCache
	path.append(L"D3DSCache");
	remove(path);

	// %LOCALAPPDATA%\NVIDIA\DXCache
	path.replace_filename(L"NVIDIA\\DXCache");
	remove(path);

	// %LOCALAPPDATA%\NVIDIA\GLCache
	path.replace_filename(L"GLCache");
	remove(path);

	SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, KF_FLAG_DEFAULT, nullptr, &known_folder_path);
	path = std::move(known_folder_path);

	// %APPDATA%\..\LocalLow\NVIDIA\PerDriverVersion\DXCache
	path.append(L"NVIDIA\\PerDriverVersion\\DXCache");
	remove(path);

	// %APPDATA%\..\LocalLow\NVIDIA\PerDriverVersion\GLCache
	path.replace_filename(L"GLCache");
	remove(path);

	SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &known_folder_path);
	path = std::move(known_folder_path);

	// %APPDATA%\..\Roaming\NVIDIA\ComputeCache
	path.append(L"NVIDIA\\ComputeCache");
	remove(path);

	system("pause");
	return 0;
}