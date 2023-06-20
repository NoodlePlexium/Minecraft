#ifndef ENGINE_DESCRIPTOR_H
#define ENGINE_DESCRIPTOR_H

 
#include "engine_device.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <stdexcept>
 
namespace Engine {
 
class EngineDescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(EngineDevice& engineDevice) : engineDevice{engineDevice} {}

        Builder& addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1)
        {
            assert(bindings.count(binding) == 0 && "Binding already in use");
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = binding;
            layoutBinding.descriptorType = descriptorType;
            layoutBinding.descriptorCount = count;
            layoutBinding.stageFlags = stageFlags;
            bindings[binding] = layoutBinding;
            return *this;
        }

        std::unique_ptr<EngineDescriptorSetLayout> build() const
        {
            return std::make_unique<EngineDescriptorSetLayout>(engineDevice, bindings);
        }

    private:
        EngineDevice& engineDevice;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
    };

    EngineDescriptorSetLayout(EngineDevice& engineDevice, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings)
        : engineDevice(engineDevice), bindings(bindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (const auto& kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(engineDevice.device(), &descriptorSetLayoutInfo, nullptr,
                                       &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    ~EngineDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(engineDevice.device(), descriptorSetLayout, nullptr);
    }

    EngineDescriptorSetLayout(const EngineDescriptorSetLayout&) = delete;
    EngineDescriptorSetLayout& operator=(const EngineDescriptorSetLayout&) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

private:
    EngineDevice& engineDevice;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class EngineDescriptorWriter;
};

 













class EngineDescriptorPool {
public:
  	class Builder {
   	public:
	    Builder(EngineDevice &engineDevice) : engineDevice{engineDevice} {}
	 
	    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count){
			poolSizes.push_back({descriptorType, count});
			return *this;
	    }

	    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags){
			poolFlags = flags;
			return *this;
	    }

	    Builder &setMaxSets(uint32_t count){
	    	maxSets = count;
	  		return *this;
	    }

	    std::unique_ptr<EngineDescriptorPool> build() const{
			return std::make_unique<EngineDescriptorPool>(engineDevice, maxSets, poolFlags, poolSizes);
	    }
 
    private:
    	EngineDevice &engineDevice;
   	 	std::vector<VkDescriptorPoolSize> poolSizes{};
    	uint32_t maxSets = 1000;
    	VkDescriptorPoolCreateFlags poolFlags = 0;
  };
 
  EngineDescriptorPool(
      	EngineDevice &engineDevice,
      	uint32_t maxSets,
      	VkDescriptorPoolCreateFlags poolFlags,
      	const std::vector<VkDescriptorPoolSize> &poolSizes) : engineDevice{engineDevice}
  	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(engineDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
  	}

  	~EngineDescriptorPool(){
		vkDestroyDescriptorPool(engineDevice.device(), descriptorPool, nullptr);
  	}

  	EngineDescriptorPool(const EngineDescriptorPool &) = delete;
  	EngineDescriptorPool &operator=(const EngineDescriptorPool &) = delete;
 
  	bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		if (vkAllocateDescriptorSets(engineDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
			return false;
		}
		return true;
  	}
 
  	void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const{
		vkFreeDescriptorSets(
			engineDevice.device(),
			descriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
  	}
 
	void resetPool(){
		vkResetDescriptorPool(engineDevice.device(), descriptorPool, 0);
	}
 
private:
  	EngineDevice &engineDevice;
  	VkDescriptorPool descriptorPool;
 
  	friend class EngineDescriptorWriter;
};
 







class EngineDescriptorWriter {
public:
	EngineDescriptorWriter(EngineDescriptorSetLayout &setLayout, EngineDescriptorPool &pool) : setLayout{setLayout}, pool{pool} {}
	 
	EngineDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo){
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
		auto &bindingDescription = setLayout.bindings[binding];
		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}
  	EngineDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo){
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
		auto &bindingDescription = setLayout.bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
		  	"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
  	}
 
  	bool build(VkDescriptorSet &set){
		bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
		if (!success) {
			return false;
		}
		overwrite(set);
		return true;
  	}
  	void overwrite(VkDescriptorSet &set){
		for (auto &write : writes) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(pool.engineDevice.device(), writes.size(), writes.data(), 0, nullptr);
  	}
 
private:
	EngineDescriptorSetLayout &setLayout;
	EngineDescriptorPool &pool;
	std::vector<VkWriteDescriptorSet> writes;
};
}  // namespace 






#endif