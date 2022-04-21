#include "descriptorSet.h"

namespace FF::Wrapper {

/*
 ���������������� descriptor set ������ɡ� ��Щ���������һ���������Ĵ洢�� ����������Ѹ�������Դ������ uniform ��������������ͼ�񣬴洢��ͼ��ȣ����ӵ���ɫ����������ͨ�����ְ󶨣�ʹ�������������ֶ���ģ�����ȡ�ͽ��ʹ������Դ���ݡ� ���磬ʹ������������Դ����ͼ�������������ͻ��������󶨵���ɫ����

 �������ǲ�͸���Ķ��󣬲�����������ɫ������ͨ�ŵ�Э�飻��Ļ�����ṩ��һ�־�Ĭ�Ļ��ƣ�����λ�ð󶨽���Դ�ڴ�����ɫ���������*/
	DescriptorSet::DescriptorSet(const Device::Ptr& device,
                                 const std::vector<UniformParameter::Ptr> params,
                                 const DescriptorSetLayout::Ptr& layout,
                                 const DescriptorPool::Ptr& pool,
                                 int frameCount
	) {
		mDevice = device;

        //��ʼ��layouts, ÿһ��ֵ����VkDescriptorSetLayout, VkDescriptorSetLayout�����������uniform����Ϣ
		std::vector<VkDescriptorSetLayout> layouts(frameCount, layout->getLayout());

        //alloc VkDescriptorSet
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool->getPool(); //���ĸ�descriptorPool����
		allocInfo.descriptorSetCount = frameCount; //��������descriptor
		allocInfo.pSetLayouts = layouts.data();//

        //����VkDescriptorSet
		mDescriptorSets.resize(frameCount);
        //����VkDescriptorSet����, ����mDescriptorSets, ����Ϊʲô����&mDescriptorSets[0]�ͻ������VkDescriptorSet����������
		if (vkAllocateDescriptorSets(mDevice->getDevice(), &allocInfo, &mDescriptorSets[0]) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to allocate descriptor sets");
		}

        
		for (int i = 0; i < frameCount; ++i) {
			//��ÿ��DescriptorSet��������Ҫ��params�����������Ϣ��д������
			std::vector<VkWriteDescriptorSet> descriptorSetWrites{};
			for (const auto& param : params) {
                //VkWriteDescriptorSetָ��binding��set������������, ������������Ű����ݴ��ݵ�buffer��uniform��sampler2D
				VkWriteDescriptorSet descriptorSetWrite{};
				descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorSetWrite.dstSet = mDescriptorSets[i];
				descriptorSetWrite.dstArrayElement = 0;
				descriptorSetWrite.descriptorType = param->mDescriptorType;
				descriptorSetWrite.descriptorCount = param->mCount;
				descriptorSetWrite.dstBinding = param->mBinding;
				
				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                    //����attribute��Ϣ��������ɫ��, VkDescriptorBufferInfo����������Ϣ
					descriptorSetWrite.pBufferInfo = &param->mBuffers[i]->getBufferInfo();
				}

				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                    //�����������ݵ�ƬԪ��ɫ��,VkDescriptorImageInfo����VkImage��VkSampler
					descriptorSetWrite.pImageInfo = &param->mTexture->getImageInfo();
				}

				descriptorSetWrites.push_back(descriptorSetWrite);
			}

            //����VkDescriptorSet������Ϣ
			vkUpdateDescriptorSets(mDevice->getDevice(), static_cast<uint32_t>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, nullptr);
		}
	}

	DescriptorSet::~DescriptorSet() {
        
    }

}
