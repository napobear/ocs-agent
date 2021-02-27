/*
 * Machine.h
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <map>
#include <string>
#include <vector>

typedef std::map<int, std::map<std::string, std::string> > dmi_db;


struct bios_info {
	std::string vendor;
	std::string release_date;
	std::string version;
	void MergeWith(const bios_info& info);
	int Score() const;
};


struct system_info {
	std::string name;
	std::string vendor;
	std::string serial;
	std::string version;
	std::string uuid;
	void MergeWith(const system_info& info);
	int Score() const;
};


struct board_info {
	std::string asset_tag;
	std::string name;
	std::string serial;
	std::string vendor;
	std::string version;
	void MergeWith(const board_info& info);
	int Score() const;
};


struct chassis_info {
	std::string asset_tag;
	std::string serial;
	std::string type;
	std::string vendor;
	std::string version;
	void MergeWith(const chassis_info& info);
	int Score() const;
};


struct video_info {
	std::string vendor;
	std::string chipset;
	std::string memory;
	std::string name;
	std::string resolution;
	void MergeWith(const video_info& info);
};


struct memory_device_info {
	memory_device_info();

	std::string Type() const;
	std::string Speed() const;
	std::string Size() const;

	std::string description;
	std::string caption;
	std::string purpose;
	std::string type;
	std::string vendor;
	std::string serial;
	std::string asset_tag;
	unsigned int speed;
	unsigned int size;
};


class OSInfo {
public:
    OSInfo();
	std::string description;
    std::string release;
	std::string hostname;
    std::string domainname;
	std::string architecture;
	std::string memory;
	std::string swap;
    std::string comments;
private:
    std::string _OSDescription();
};


class Machine {
public:
	static Machine* Get();

	std::string AssetTag() const;
	std::string BIOSManufacturer() const;
	std::string BIOSDate() const;
	std::string BIOSVersion() const;

	std::string MachineManufacturer() const;
	std::string MachineSerialNumber() const;

	std::string SystemModel() const;
	std::string SystemSerialNumber() const;
	std::string SystemUUID() const;
	std::string SystemManufacturer() const;
	std::string SystemType() const;

	int CountMemories();
	std::string MemoryCaption(int num);
	std::string MemoryDescription(int num);
	std::string MemoryCapacity(int num);
	std::string MemoryPurpose(int num);
	std::string MemoryType(int num);
	std::string MemorySpeed(int num);
	std::string MemoryNumSlot(int num);
	std::string MemorySerialNumber(int num);

	int CountVideos() const;
	video_info VideoInfoFor(int numVideo) const;

private:
	Machine();
	~Machine();

	void _RetrieveData();
	bool _GetDMIData();
	bool _GetGraphicsCardInfo();
	bool _GetDMIDecodeData();
	bool _GetLSHWData();
	void _ExtractDataFromDMIDB(dmi_db dmiDb);

	std::vector<memory_device_info> fMemoryInfo;

	bios_info fBIOSInfo;
	chassis_info fChassisInfo;
	board_info fBoardInfo;
	system_info fSystemInfo;

	std::vector<struct video_info> fVideoInfo;
};

#endif /* MACHINE_H_ */
