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
#include <fastrtps/subscriber/SampleInfo.h>

#include <fastrtps/types/DynamicDataFactory.h>
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/DynamicDataPtr.h>
#include <fastrtps/types/TypesBase.h>

#include <iostream>
#include <map>
#include <vector>

using namespace eprosima;
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

private:
    void internal_notify_type_object_( DynamicType_ptr dynamic_type);
  

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

        void on_type_discovery(
            DomainParticipant* participant,
            const fastrtps::rtps::SampleIdentity& request_sample_id,
            const fastrtps::string_255& topic_name,
            const fastrtps::types::TypeIdentifier* identifier,
            const fastrtps::types::TypeObject* object,
            fastrtps::types::DynamicType_ptr dyn_type)  {

            if (nullptr != dyn_type)
            {
                // Register type obj in singleton factory
                TypeObjectFactory::get_instance()->add_type_object(
                    dyn_type->get_name(), identifier, object);
                std::cout << "---- on_type_discovery dyn type: " << dyn_type->get_name() << std::endl;
                // internal_notify_type_object_(dyn_type);

                // //CREATE THE TOPIC
                // eprosima::fastdds::dds::Topic* topic = subscriber_->mp_participant->create_topic(
                //     std::string(topic_name), dyn_type->get_name(), TOPIC_QOS_DEFAULT);

                // StatusMask sub_mask = StatusMask::subscription_matched() << StatusMask::data_available();
                // DataReader* reader = subscriber_->mp_subscriber->create_datareader(
                //     topic,
                //     subscriber_->qos_,
                //     &subscriber_->m_listener,
                //     sub_mask);

                // subscriber_->topics_[reader] = topic;
                // subscriber_->readers_[reader] = dyn_type;
                // fastrtps::types::DynamicData_ptr data(fastrtps::types::DynamicDataFactory::get_instance()->create_data(dyn_type));
                // subscriber_->datas_[reader] = data;
            }
        
        }


        void on_type_dependencies_reply(
            DomainParticipant* participant,
            const fastrtps::rtps::SampleIdentity& request_sample_id,
            const fastrtps::types::TypeIdentifierWithSizeSeq& dependencies) {
            //printf("---- on_type_dependencies_reply. \n");
        }

        void on_type_information_received(
                eprosima::fastdds::dds::DomainParticipant* participant,
                const eprosima::fastrtps::string_255 topic_name,
                const eprosima::fastrtps::string_255 type_name,
                const eprosima::fastrtps::types::TypeInformation& type_information) override;


        void on_participant_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::ParticipantDiscoveryInfo&& info) {
            // printf("---- find participant. name:%s \n", info.info.m_participantName.c_str());
        }


        void on_publisher_discovery(
            DomainParticipant* participant,
            fastrtps::rtps::WriterDiscoveryInfo&& info) override {
            //  printf("---- find publisher. topic_name:%s  topic_type:%s \n", 
            //     info.info.topicName().c_str(), info.info.typeName().c_str());

        }

        void on_data_available(
                eprosima::fastdds::dds::DataReader* reader) override;

        void on_subscription_matched(
                eprosima::fastdds::dds::DataReader* reader,
                const eprosima::fastdds::dds::SubscriptionMatchedStatus& info) override;

        int n_matched;
        uint32_t n_samples;
        EchoTool* subscriber_;
        std::map<std::string, std::string> topic_type_map_;
        eprosima::fastrtps::types::TypeInformation type_info_;
    }
    m_listener;

};
