#pragma once
#include <Windows.h>
#include <intrin.h>
#include <bitset>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

class HypervisorInfo
{
	template <typename T>
	std::string valueToHexString(T value) {
		std::ostringstream hexStream;
		hexStream << std::hex << value;
		return "0x" + hexStream.str();
	}

	struct Registers {
		int eax = 0;
		int ebx = 0;
		int ecx = 0;
		int edx = 0;
	};

	Registers QueryCPUIDLeaf(int leaf) {
		Registers registers;
		
		if (leaf < 0x40000000 || this->m_MaxLeaf >= leaf) {
			__cpuid(&registers.eax, leaf);
			return registers;
		}
		else {
			std::string err = "Leaf " + valueToHexString(leaf) + " not supported. ";
			err += "(Max: " + valueToHexString(this->m_MaxLeaf) + ')';
			throw std::runtime_error(err);
		}
	}

	void GetMaxLeafAndVendorID(int& maxLeaf, std::string& vendorID) {
		Registers registers;
		char buffer[12] = { 0 };

		try {
			registers = QueryCPUIDLeaf(0x40000000);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}

		maxLeaf = registers.eax;
		memcpy(buffer, &registers.ebx, 4);
		memcpy(buffer + 4, &registers.ecx, 4);
		memcpy(buffer + 8, &registers.edx, 4);

		vendorID.assign(buffer, sizeof(buffer));
	}

	void GetInterfaceID(std::string& interfaceID) {
		Registers registers;
		char buffer[4] = { 0 };

		try {
			registers = QueryCPUIDLeaf(0x40000001);
			memcpy(buffer, &registers.eax, 4);
			//buffer[4] = '\0';

			interfaceID.assign(buffer, sizeof(buffer));
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}
	}

	struct BuildInfo {
		int BuildNumber;
		short MajorVersion;
		short MinorVersion;
		std::string BuildString;
	};

	void GetBuildInfo(BuildInfo& buildInfo) {
		Registers registers;

		try {
			registers = QueryCPUIDLeaf(0x40000002);

			buildInfo.BuildNumber = registers.eax;
			buildInfo.MajorVersion = registers.ebx >> 16;
			buildInfo.MinorVersion = registers.ebx & 0xFFFF;
			buildInfo.BuildString =
				std::to_string(buildInfo.MajorVersion) + "." +
				std::to_string(buildInfo.MinorVersion) + "." +
				std::to_string(buildInfo.BuildNumber);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}
	}

	struct HypervisorFeaturesEnabled {
		// Virtual MSRs
		bool AccessVpRunTimeReg = false;
		bool AccessPartitionReferenceCounter = false;
		bool AccessSynicRegs = false;
		bool AccessSyntheticTimerRegs = false;
		bool AccessIntrCtrlRegs = false;
		bool AccessHypercallMsrs = false;
		bool AccessVpIndex = false;
		bool AccessResetReg = false;
		bool AccessStatsReg = false;
		bool AccessPartitionReferenceTsc = false;
		bool AccessGuestIdleReg = false;
		bool AccessFrequencyRegs = false;
		bool AccessDebugRegs = false;
		bool AccessReenlightenmentControls = false;
		// Hypercalls
		bool CreatePartitions = false;
		bool AccessPartitionId = false;
		bool AccessMemoryPool = false;
		bool PostMessages = false;
		bool SignalEvents = false;
		bool CreatePort = false;
		bool ConnectPort = false;
		bool AccessStats = false;
		bool Debugging = false;
		bool CpuManagement = false;
		bool AccessVSM = false;
		bool AccessVpRegisters = false;
		bool EnableExtendedHypercalls = false;
		bool StartVirtualProcessor = false;

		bool InvarientMperf = false;
		bool SupervisorShadowStack = false;
		bool ArchitecturalPMU = false;
		bool ExceptionTrapIntercept = false;

		bool GuestDebugging = false;
		bool PerformanceMonitorSupport = false;
		bool PhysicalCPUDynamicPartitioningEvents = false;
		bool HypercallInputParametersXMM = false;
		bool VirtualGuestIdleState = false;
		bool HypervisorSleepState = false;
		bool NumaDistanceQuery = false;
		bool TimerFrequencies = false;
		bool SyntheticMachineCheck = false;
		bool GuestCrashMSR = false;
		bool DebugMSR = false;
		bool NPIEP = false;
		bool DisableHypervisor = false;
		bool ExtendedGvaRangesForFlushVirtualAddressList = false;
		bool HypercallOutputParametersXMM = false;
		bool SintPollingMode = false;
		bool HypercallMsrLock = false;
		bool DirectSyntheticTimers = false;
		bool PATRegister = false;
		bool BNDCFGSRegister = false;
		bool SyntheticTimeUnhaltedTimer = false;
		bool IntelLBR = false;
	};

	void GetHypervisorFeatures(HypervisorFeaturesEnabled& hvFeatures) {
		Registers registers;

		try {
			registers = QueryCPUIDLeaf(0x40000003);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}

		std::bitset<32> hvPartitionPrivilegeMaskLow(registers.eax);
		hvFeatures.AccessVpRunTimeReg = hvPartitionPrivilegeMaskLow[0];
		hvFeatures.AccessPartitionReferenceCounter = hvPartitionPrivilegeMaskLow[1];
		hvFeatures.AccessSynicRegs = hvPartitionPrivilegeMaskLow[2];
		hvFeatures.AccessSyntheticTimerRegs = hvPartitionPrivilegeMaskLow[3];
		hvFeatures.AccessIntrCtrlRegs = hvPartitionPrivilegeMaskLow[4];
		hvFeatures.AccessHypercallMsrs = hvPartitionPrivilegeMaskLow[5];
		hvFeatures.AccessVpIndex = hvPartitionPrivilegeMaskLow[6];
		hvFeatures.AccessResetReg = hvPartitionPrivilegeMaskLow[7];
		hvFeatures.AccessStatsReg = hvPartitionPrivilegeMaskLow[8];
		hvFeatures.AccessPartitionReferenceTsc = hvPartitionPrivilegeMaskLow[9];
		hvFeatures.AccessGuestIdleReg = hvPartitionPrivilegeMaskLow[10];
		hvFeatures.AccessFrequencyRegs = hvPartitionPrivilegeMaskLow[11];
		hvFeatures.AccessDebugRegs = hvPartitionPrivilegeMaskLow[12];
		hvFeatures.AccessReenlightenmentControls = hvPartitionPrivilegeMaskLow[13];

		std::bitset<32> hvPartitionPrivilegeMaskHigh(registers.ebx);
		hvFeatures.CreatePartitions = hvPartitionPrivilegeMaskHigh[0];
		hvFeatures.AccessPartitionId = hvPartitionPrivilegeMaskHigh[1];
		hvFeatures.AccessMemoryPool = hvPartitionPrivilegeMaskHigh[2];
		hvFeatures.PostMessages = hvPartitionPrivilegeMaskHigh[4];
		hvFeatures.SignalEvents = hvPartitionPrivilegeMaskHigh[5];
		hvFeatures.CreatePort = hvPartitionPrivilegeMaskHigh[6];
		hvFeatures.ConnectPort = hvPartitionPrivilegeMaskHigh[7];
		hvFeatures.AccessStats = hvPartitionPrivilegeMaskHigh[8];
		hvFeatures.Debugging = hvPartitionPrivilegeMaskHigh[11];
		hvFeatures.CpuManagement = hvPartitionPrivilegeMaskHigh[12];
		hvFeatures.AccessVSM = hvPartitionPrivilegeMaskHigh[16];
		hvFeatures.AccessVpRegisters = hvPartitionPrivilegeMaskHigh[17];
		hvFeatures.EnableExtendedHypercalls = hvPartitionPrivilegeMaskHigh[20];
		hvFeatures.StartVirtualProcessor = hvPartitionPrivilegeMaskHigh[21];

		std::bitset<32> hvFeaturesECX(registers.ecx);
		hvFeatures.InvarientMperf = hvFeaturesECX[5];
		hvFeatures.SupervisorShadowStack = hvFeaturesECX[6];
		hvFeatures.ArchitecturalPMU = hvFeaturesECX[7];
		hvFeatures.ExceptionTrapIntercept = hvFeaturesECX[8];

		std::bitset<32> hvFeaturesEDX(registers.edx);
		hvFeatures.GuestDebugging = hvFeaturesEDX[1];
		hvFeatures.PerformanceMonitorSupport = hvFeaturesEDX[2];
		hvFeatures.PhysicalCPUDynamicPartitioningEvents = hvFeaturesEDX[3];
		hvFeatures.HypercallInputParametersXMM = hvFeaturesEDX[4];
		hvFeatures.VirtualGuestIdleState = hvFeaturesEDX[5];
		hvFeatures.HypervisorSleepState = hvFeaturesEDX[6];
		hvFeatures.NumaDistanceQuery = hvFeaturesEDX[7];
		hvFeatures.TimerFrequencies = hvFeaturesEDX[8];
		hvFeatures.SyntheticMachineCheck = hvFeaturesEDX[9];
		hvFeatures.GuestCrashMSR = hvFeaturesEDX[10];
		hvFeatures.DebugMSR = hvFeaturesEDX[11];
		hvFeatures.NPIEP = hvFeaturesEDX[12];
		hvFeatures.DisableHypervisor = hvFeaturesEDX[13];
		hvFeatures.ExtendedGvaRangesForFlushVirtualAddressList = hvFeaturesEDX[14];
		hvFeatures.HypercallOutputParametersXMM = hvFeaturesEDX[15];
		hvFeatures.SintPollingMode = hvFeaturesEDX[17];
		hvFeatures.HypercallMsrLock = hvFeaturesEDX[18];
		hvFeatures.DirectSyntheticTimers = hvFeaturesEDX[19];
		hvFeatures.PATRegister = hvFeaturesEDX[20];
		hvFeatures.BNDCFGSRegister = hvFeaturesEDX[21];
		hvFeatures.SyntheticTimeUnhaltedTimer = hvFeaturesEDX[23];
		hvFeatures.IntelLBR = hvFeaturesEDX[26];
	}

	struct HardwareFeatures {
		bool APICOverlayAssist = false;
		bool MSRBitmaps = false;
		bool ArchitecturalPerformanceCounters = false;
		bool SLAT = false;
		bool DMARemapping = false;
		bool InterruptRemapping = false;
		bool MemoryPatrolScrubber = false;
		bool DMAProtection = false;
		bool HPET = false;
		bool VolatileSyntheticTimers = false;
		uint32_t HypervisorLevel = 0;
		bool PhysicalDestinationMode = false;
		bool HardwareMemoryZeroing = false;
		bool UnrestrictedGuest = false;
		bool ResourceAllocation = false;
		bool ResourceMonitoring = false;
		bool GuestVirtualPMU = false;
		bool GuestVirtualLBR = false;
		bool GuestVirtualIPT = false;
		bool APICEmulation = false;
		bool ACPIWDAT = false;
	};

	void GetHardwareFeatures(HardwareFeatures& hwFeatures) {
		Registers registers;
		try {
			registers = QueryCPUIDLeaf(0x40000006);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}
		
		std::bitset<32> hwFeaturesEAX(registers.eax);
		hwFeatures.APICOverlayAssist = hwFeaturesEAX[0];
		hwFeatures.MSRBitmaps = hwFeaturesEAX[1];
		hwFeatures.ArchitecturalPerformanceCounters = hwFeaturesEAX[2];
		hwFeatures.SLAT = hwFeaturesEAX[3];
		hwFeatures.DMARemapping = hwFeaturesEAX[4];
		hwFeatures.InterruptRemapping = hwFeaturesEAX[5];
		hwFeatures.MemoryPatrolScrubber = hwFeaturesEAX[6];
		hwFeatures.DMAProtection = hwFeaturesEAX[7];
		hwFeatures.HPET = hwFeaturesEAX[8];
		hwFeatures.VolatileSyntheticTimers = hwFeaturesEAX[9];
		hwFeatures.HypervisorLevel = (hwFeaturesEAX.to_ulong() >> 10) & 0x0F;
		hwFeatures.PhysicalDestinationMode = hwFeaturesEAX[14];
		hwFeatures.HardwareMemoryZeroing = hwFeaturesEAX[16];
		hwFeatures.UnrestrictedGuest = hwFeaturesEAX[17];
		hwFeatures.ResourceAllocation = hwFeaturesEAX[18];
		hwFeatures.ResourceMonitoring = hwFeaturesEAX[19];
		hwFeatures.GuestVirtualPMU = hwFeaturesEAX[20];
		hwFeatures.GuestVirtualLBR = hwFeaturesEAX[21];
		hwFeatures.GuestVirtualIPT = hwFeaturesEAX[22];
		hwFeatures.APICEmulation = hwFeaturesEAX[23];
		hwFeatures.ACPIWDAT = hwFeaturesEAX[24];
	}

	struct EnlightenmentInfo {
		bool HypercallForAddressSpaceSwitches = false;
		bool HypercallForLocalTLBFlush = false;
		bool HypercallForRemoteTLBFlush = false;
		bool MSRForAPICAccess = false;
		bool MSRForSystemReset = false;
		bool UseRelaxedTiming = false;
		bool UseDMARemapping = false;
		bool UseInterruptRemapping = false;
		bool DeprecateAutoEOI = false;
		bool UseSyntheticClusterIPI = false;
		bool UseExProcessorMasks = false;
		bool HypervisorIsNested = false;
		bool UseIntForMBEC = false;
		bool UseEnlightenedVMCS = false;
		bool UseSyncedTimeline = false;
		bool UseDirectLocalFlushEntire = false;
		bool NoNonArchitecturalCoreSharing = false;

		unsigned int MaxSpinlockRetries = 0;

		unsigned short ImplementedPhysicalAddressBits = 0;

	};

	void GetEnlightenmentInformation(EnlightenmentInfo& enlightenmentInfo) {
		Registers registers;
		
		try {
			registers = QueryCPUIDLeaf(0x40000004);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}

		std::bitset<32> enlightenmentInfoEAX(registers.eax);
		enlightenmentInfo.HypercallForAddressSpaceSwitches = enlightenmentInfoEAX[0];
		enlightenmentInfo.HypercallForLocalTLBFlush = enlightenmentInfoEAX[1];
		enlightenmentInfo.HypercallForRemoteTLBFlush = enlightenmentInfoEAX[2];
		enlightenmentInfo.MSRForAPICAccess = enlightenmentInfoEAX[3];
		enlightenmentInfo.MSRForSystemReset = enlightenmentInfoEAX[4];
		enlightenmentInfo.UseRelaxedTiming = enlightenmentInfoEAX[5];
		enlightenmentInfo.UseDMARemapping = enlightenmentInfoEAX[6];
		enlightenmentInfo.UseInterruptRemapping = enlightenmentInfoEAX[7];
		enlightenmentInfo.DeprecateAutoEOI = enlightenmentInfoEAX[9];
		enlightenmentInfo.UseSyntheticClusterIPI = enlightenmentInfoEAX[10];
		enlightenmentInfo.UseExProcessorMasks = enlightenmentInfoEAX[11];
		enlightenmentInfo.HypervisorIsNested = enlightenmentInfoEAX[12];
		enlightenmentInfo.UseIntForMBEC = enlightenmentInfoEAX[13];
		enlightenmentInfo.UseEnlightenedVMCS = enlightenmentInfoEAX[14];
		enlightenmentInfo.UseSyncedTimeline = enlightenmentInfoEAX[15];
		enlightenmentInfo.UseDirectLocalFlushEntire = enlightenmentInfoEAX[17];
		enlightenmentInfo.NoNonArchitecturalCoreSharing = enlightenmentInfoEAX[18];

		enlightenmentInfo.MaxSpinlockRetries = registers.ebx;

		std::bitset<32> enlightenmentInfoECX(registers.ecx);
		enlightenmentInfo.ImplementedPhysicalAddressBits = enlightenmentInfoECX.to_ulong() & 0x7F;
	}

	struct ImplementationLimits {
		unsigned int MaxVirtualProcessors = 0;
		unsigned int MaxLogicalProcessors = 0;
		unsigned int MaxPhysicalInterruptVectors = 0;
	};

	void GetImplementationLimits(ImplementationLimits& implementationLimits) {
		Registers registers;
		try {
			registers = QueryCPUIDLeaf(0x40000005);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}

		implementationLimits.MaxVirtualProcessors = registers.eax;
		implementationLimits.MaxLogicalProcessors = registers.ebx;
		implementationLimits.MaxPhysicalInterruptVectors = registers.ecx;
	}

public:
	bool m_IsHypervisorPresent = false;
	int m_MaxLeaf = INT_MAX;
	std::string m_VendorID;
	std::string m_InterfaceID;
	bool m_IsMicrosoftHypervisor = false;
	BuildInfo m_BuildInfo;
	HypervisorFeaturesEnabled m_HypervisorFeaturesEnabled;
	HardwareFeatures m_HardwareFeatures;
	EnlightenmentInfo m_EnlightenmentInfo;
	ImplementationLimits m_ImplementationLimits;

	HypervisorInfo() {
		Registers registers = QueryCPUIDLeaf(1);
		this->m_IsHypervisorPresent = registers.ecx & (1 << 31);
		// If no hypervisor was detected then we should bail out here
		if (!this->m_IsHypervisorPresent) {
			return;
		}

		// HviGetHypervisorVendorAndMaxFunction
		GetMaxLeafAndVendorID(this->m_MaxLeaf, this->m_VendorID);
		
		try {
			// HviGetHypervisorInterface
			GetInterfaceID(this->m_InterfaceID);
			if (this->m_InterfaceID == "Hv#1") {
				this->m_IsMicrosoftHypervisor = true;
			}
			else {
				// Because we're not guaranteed to have any other leaves,
				// we'll just return here
				std::string message = "Not a TLFS-compliant hypervisor (ID: " + std::string(this->m_InterfaceID) + ").";
				message += " Exiting after querying leaf 0x40000001.";
				throw std::runtime_error(message.c_str());
			}

			// HviGetHypervisorVersion
			GetBuildInfo(this->m_BuildInfo);

			// HviGetHypervisorFeatures
			GetHypervisorFeatures(this->m_HypervisorFeaturesEnabled);

			// HviGetHardwareFeatures
			GetHardwareFeatures(this->m_HardwareFeatures);

			// HviGetEnlightenmentInformation
			GetEnlightenmentInformation(this->m_EnlightenmentInfo);

			// HviGetImplementationLimits
			GetImplementationLimits(this->m_ImplementationLimits);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			return;
		}
	}
};
