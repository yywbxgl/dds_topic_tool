#pragma once

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>

#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>


#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/DynamicDataHelper.hpp>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/TypeObjectFactory.h>

#include <iostream>
#include <map>
#include <vector>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::types;


class EchoTool
{
public:
    EchoTool() 
    : mp_participant(nullptr)
    , mp_subscriber(nullptr)
    , m_listener(this) {}

    ~EchoTool();

    void init();

    void run();

    void subscribe_to_topic(const std::string& topic_name);

public:
    std::string sub_topic_;
    std::vector<std::string> sub_topic_list_;

private:
    // DomainParticipant* participant_;
    // Subscriber* subscriber_;
    // std::map<std::string, Topic*> topics_;
    // std::map<std::string, DataReader*> readers_;
    // std::map<std::string, DynamicTypeHandle*> type_handles_;

    eprosima::fastdds::dds::DomainParticipant* mp_participant;
    eprosima::fastdds::dds::Subscriber* mp_subscriber;
    std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastdds::dds::Topic*> topics_;
    std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastrtps::types::DynamicType_ptr> readers_;
    std::map<eprosima::fastdds::dds::DataReader*, eprosima::fastrtps::types::DynamicData_ptr> datas_;
    eprosima::fastrtps::SubscriberAttributes att_;
    eprosima::fastdds::dds::DataReaderQos qos_;
    eprosima::fastrtps::types::DynamicPubSubType m_type;

public:
    class SubListener: public DomainParticipantListener {
    public:
        SubListener(EchoTool* sub)
            : n_matched(0)
            , n_samples(0)
            , subscriber_(sub) {}

        ~SubListener() override {}

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

        int n_matched;
        uint32_t n_samples;
        EchoTool* subscriber_;
        std::map<std::string, std::string> topic_type_map_;
        eprosima::fastrtps::types::TypeInformation type_info_;
    }
    m_listener;

};
