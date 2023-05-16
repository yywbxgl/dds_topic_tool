// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file ListTool.h
 *
 */

#pragma once

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/rtps/common/Types.h>

#include <fastrtps/types/DynamicPubSubType.h>
#include <fastrtps/types/DynamicTypePtr.h>

#include <fastrtps/attributes/SubscriberAttributes.h>

#include <map>
#include <string>

using namespace eprosima;
using namespace eprosima::fastdds::dds;



class ListTool
{
public:

    ListTool() {}

    virtual ~ListTool() {}

    bool init() {
        DomainParticipantQos pqos;
        pqos.name("DDS List Tool");

        //Do not enable entities on creation
        DomainParticipantFactoryQos factory_qos;
        factory_qos.entity_factory().autoenable_created_entities = false;
        DomainParticipantFactory::get_instance()->set_qos(factory_qos);

        StatusMask par_mask = StatusMask::subscription_matched() << StatusMask::data_available();
        eprosima::fastdds::dds::DomainParticipant* mp_participant = 
            DomainParticipantFactory::get_instance()->create_participant(0, pqos, &m_listener, par_mask);
        if (mp_participant == nullptr){
            return false;
        }
        if (mp_participant->enable() != ReturnCode_t::RETCODE_OK){
            DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
            return false;
        }

        // qos_ = DATAREADER_QOS_DEFAULT;
        return true;
    }



    void run(int t_flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for(auto i: m_listener.topic_type_map_) {
            if (t_flag)
                printf("\e[32m%s \t[%s]\e[0m\n", i.first.c_str(), i.second.c_str());
            else
                printf("\e[32m%s \e[0m\n", i.first.c_str());
        }
    }


private:

    // eprosima::fastdds::dds::DomainParticipant* mp_participant;
    // eprosima::fastdds::dds::Subscriber* mp_subscriber;
    // std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastdds::dds::Topic*> topics_;
    // std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastrtps::types::DynamicType_ptr> readers_;
    // std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastrtps::types::DynamicData_ptr> datas_;
    // eprosima::fastrtps::SubscriberAttributes att_;
    // eprosima::fastdds::dds::DataReaderQos qos_;
    // eprosima::fastrtps::types::DynamicPubSubType m_type;

public:

    class SubListener
        : public eprosima::fastdds::dds::DomainParticipantListener
    {
    public:

        SubListener() {}

        ~SubListener() override{}

        void on_type_dependencies_reply(
            DomainParticipant* participant,
            const fastrtps::rtps::SampleIdentity& request_sample_id,
            const fastrtps::types::TypeIdentifierWithSizeSeq& dependencies) {
            // printf("---- on_type_dependencies_reply. \n");
        }


        void on_type_information_received(
                eprosima::fastdds::dds::DomainParticipant* participant,
                const eprosima::fastrtps::string_255 topic_name,
                const eprosima::fastrtps::string_255 type_name,
                const eprosima::fastrtps::types::TypeInformation& type_information) override {
            // printf("---- on_type_information_received. \n");
        }

        void on_type_discovery(
            DomainParticipant* participant,
            const fastrtps::rtps::SampleIdentity& request_sample_id,
            const fastrtps::string_255& topic,
            const fastrtps::types::TypeIdentifier* identifier,
            const fastrtps::types::TypeObject* object,
            fastrtps::types::DynamicType_ptr dyn_type)  {
            // printf("---- on_type_discovery. \n");
        }
    

        void on_participant_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::ParticipantDiscoveryInfo&& info) {
            // printf("find participant. name:%s \n", info.info.m_participantName.c_str());
        }

        void on_subscriber_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::ReaderDiscoveryInfo&& info) override {
            //  printf("find subscriber. topic_name:%s  topic_type:%s \n", 
            //     info.info.topicName().c_str(), info.info.typeName().c_str());
            std::string topic_name = std::string(info.info.topicName());
            std::string topic_type =  std::string(info.info.typeName());
            if (topic_name.find("ros") ==  std::string::npos)
                topic_type_map_[topic_name] = topic_type;
        }


        void on_publisher_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::WriterDiscoveryInfo&& info) override {
            //  printf("find publisher. topic_name:%s  topic_type:%s \n", 
            //     info.info.topicName().c_str(), info.info.typeName().c_str());
            std::string topic_name =  std::string(info.info.topicName());
            std::string topic_type =  std::string(info.info.typeName());
            if (topic_name.find("ros") ==  std::string::npos)
                topic_type_map_[topic_name] = topic_type;
        }

        std::map<std::string, std::string> topic_type_map_;
    }
    m_listener;
   
};

