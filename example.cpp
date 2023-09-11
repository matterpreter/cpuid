#include "cpuid.hpp"
#include <iostream>

int main() {
	HypervisorInfo hvInfo;

	if (hvInfo.m_IsHypervisorPresent) {
		std::cout << "Hypervisor detected" << std::endl;
		std::cout << "Max leaf: 0x" << std::hex << hvInfo.m_MaxLeaf << std::endl;
		std::cout << "Vendor ID: " << hvInfo.m_VendorID << std::endl;
		std::cout << "Interface ID: " << hvInfo.m_InterfaceID << std::endl;
		std::cout << "Build Number: " << hvInfo.m_BuildInfo.BuildString << std::endl;
		std::cout << "Guest debugging enabled: " << (hvInfo.m_HypervisorFeaturesEnabled.GuestDebugging ? "true" : "false") << std::endl;
		std::cout << "SLAT present: " << (hvInfo.m_HardwareFeatures.SLAT ? "true" : "false") << std::endl;
		std::cout << "Hypervisor level: " << hvInfo.m_HardwareFeatures.HypervisorLevel << std::endl;
		std::cout << "Extended hypercalls enabled: " << (hvInfo.m_HypervisorFeaturesEnabled.EnableExtendedHypercalls ? "true" : "false") << std::endl;
		std::cout << "SLAT enabled: " << (hvInfo.m_HardwareFeatures.SLAT ? "true" : "false") << std::endl;
		std::cout << "Hypervisor is nested: " << (hvInfo.m_EnlightenmentInfo.HypervisorIsNested ? "true" : "false") << std::endl;
		std::cout << "Max virtual processors: " << hvInfo.m_ImplementationLimits.MaxVirtualProcessors << std::endl;
		std::cout << "Max logical processors: " << hvInfo.m_ImplementationLimits.MaxLogicalProcessors << std::endl;
		std::cout << "Max physical interrupt vectors: " << hvInfo.m_ImplementationLimits.MaxPhysicalInterruptVectors << std::endl;
	}
	else {
		std::cout << "Hypervisor not detected" << std::endl;
	}
}
