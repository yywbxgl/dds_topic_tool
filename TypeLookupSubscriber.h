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
 * @file TypeLookupSubscriber.h
 *
 */

#ifndef HELLOWORLDSUBSCRIBER_H_
#define HELLOWORLDSUBSCRIBER_H_

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



class TypeLookupSubscriber
{
public:

    TypeLookupSubscriber();

    virtual ~TypeLookupSubscriber();

    //!Initialize the subscriber
    bool init();

    //!RUN the subscriber
    void run();

    //!Run the subscriber until number samples have been received.
    void run(
            uint32_t number);

private:

    eprosima::fastdds::dds::DomainParticipant* mp_participant;

    eprosima::fastdds::dds::Subscriber* mp_subscriber;

    std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastdds::dds::Topic*> topics_;

    std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastrtps::types::DynamicType_ptr> readers_;

    std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastrtps::types::DynamicData_ptr> datas_;

    eprosima::fastrtps::SubscriberAttributes att_;

    eprosima::fastdds::dds::DataReaderQos qos_;

public:

    class SubListener
        : public eprosima::fastdds::dds::DomainParticipantListener
    {
    public:

        SubListener(
                TypeLookupSubscriber* sub)
            : n_matched(0)
            , n_samples(0)
            , subscriber_(sub)
        {
        }

        ~SubListener() override
        {
        }

        void on_data_available(
                eprosima::fastdds::dds::DataReader* reader) override;

        void on_subscription_matched(
                eprosima::fastdds::dds::DataReader* reader,
                const eprosima::fastdds::dds::SubscriptionMatchedStatus& info) override;

        void on_type_information_received(
                eprosima::fastdds::dds::DomainParticipant* participant,
                const eprosima::fastrtps::string_255 topic_name,
                const eprosima::fastrtps::string_255 type_name,
                const eprosima::fastrtps::types::TypeInformation& type_information) override;

        void on_type_discovery(
            DomainParticipant* participant,
            const fastrtps::rtps::SampleIdentity& request_sample_id,
            const fastrtps::string_255& topic,
            const fastrtps::types::TypeIdentifier* identifier,
            const fastrtps::types::TypeObject* object,
            fastrtps::types::DynamicType_ptr dyn_type)  
        {
            printf("---- on_type_discovery. \n");
        }
    

        void on_participant_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::ParticipantDiscoveryInfo&& info)
        {
            // printf("find participant. name:%s \n", info.info.m_participantName.c_str());
        }

        void on_subscriber_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::ReaderDiscoveryInfo&& info) override
        {
            //  printf("find subscriber. topic_name:%s  topic_type:%s \n", 
            //     info.info.topicName().c_str(), info.info.typeName().c_str());
            std::string topic_name = std::string(info.info.topicName());
            std::string topic_type =  std::string(info.info.topicName());
            if (topic_name.find("ros") ==  std::string::npos)
                topic_type_map_[topic_name] = topic_type;
        }


        void on_publisher_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::WriterDiscoveryInfo&& info) override
        {
            //  printf("find publisher. topic_name:%s  topic_type:%s \n", 
            //     info.info.topicName().c_str(), info.info.typeName().c_str());
            std::string topic_name =  std::string(info.info.topicName());
            std::string topic_type =  std::string(info.info.topicName());
            if (topic_name.find("ros") ==  std::string::npos)
                topic_type_map_[topic_name] = topic_type;
        }

        int n_matched;

        uint32_t n_samples;

        TypeLookupSubscriber* subscriber_;

        std::map<std::string, std::string> topic_type_map_;

        eprosima::fastrtps::types::TypeInformation type_info_;

        
    }
    m_listener;

private:

    eprosima::fastrtps::types::DynamicPubSubType m_type;
};

#endif /* HELLOWORLDSUBSCRIBER_H_ */
