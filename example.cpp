#include "cpuid.hpp"
#include <iostream>

void main() {
	HypervisorInfo hvInfo;

	if (hvInfo.m_IsHypervisorPresent) {
		std::cout << "[+] Hypervisor detected" << std::endl;
		std::cout << "Max leaf: 0x" << std::hex << hvInfo.m_MaxLeaf << std::endl;
		std::cout << "Vendor ID: " << hvInfo.m_VendorID << std::endl;
		std::cout << "Interface ID: " << hvInfo.m_InterfaceID << std::endl;

		if (hvInfo.m_IsMicrosoftHypervisor) { // Interface Hv#1 is exposed
			std::cout << "Build Number: " << hvInfo.m_BuildInfo.BuildString << std::endl;
			std::cout << "SLAT present: " << std::boolalpha << hvInfo.m_HardwareFeatures.SLAT << std::endl;
			std::cout << "SLAT enabled: " << std::boolalpha << hvInfo.m_HardwareFeatures.SLAT << std::endl;
			std::cout << "Extended hypercalls enabled: " << std::boolalpha << hvInfo.m_HypervisorFeaturesEnabled.EnableExtendedHypercalls << std::endl;
			std::cout << "Hypervisor is nested: " << std::boolalpha << hvInfo.m_EnlightenmentInfo.HypervisorIsNested << std::endl;
			std::cout << "Hypervisor level: " << hvInfo.m_HardwareFeatures.HypervisorLevel << std::endl;
			std::cout << "Guest debugging enabled: " << std::boolalpha << hvInfo.m_HypervisorFeaturesEnabled.GuestDebugging << std::endl;
			std::cout << "Max virtual processors: " << hvInfo.m_ImplementationLimits.MaxVirtualProcessors << std::endl;
			std::cout << "Max logical processors: " << hvInfo.m_ImplementationLimits.MaxLogicalProcessors << std::endl;
		}
	}
	else {
		std::cout << "[-] Hypervisor not detected" << std::endl;
	}
}
