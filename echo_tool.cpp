#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>

#include <iostream>
#include <map>

#include "echo_tool.h"

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps;


void EchoTool::init()
{
    // Create the participant and subscriber
    DomainParticipantQos pqos;
    pqos.wire_protocol().builtin.discovery_config.discoveryProtocol =  eprosima::fastrtps::rtps::SIMPLE;
    pqos.wire_protocol().builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
    pqos.wire_protocol().builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    pqos.wire_protocol().builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    pqos.wire_protocol().builtin.typelookup_config.use_client = true;
    pqos.wire_protocol().builtin.use_WriterLivelinessProtocol = false;
    pqos.wire_protocol().builtin.discovery_config.leaseDuration = c_TimeInfinite;
    pqos.name("DDS Echo Tool");

    DomainParticipantFactoryQos factory_qos;
    factory_qos.entity_factory().autoenable_created_entities = false;
    DomainParticipantFactory::get_instance()->set_qos(factory_qos);

    StatusMask par_mask = StatusMask::subscription_matched() << StatusMask::data_available();
    mp_participant = DomainParticipantFactory::get_instance()->create_participant(0, pqos, &m_listener, par_mask);
    if (mp_participant == nullptr)
    {
        return ;
    }
    if (mp_participant->enable() != ReturnCode_t::RETCODE_OK)
    {
        DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
        return ;
    }

    factory_qos.entity_factory().autoenable_created_entities = true;
    DomainParticipantFactory::get_instance()->set_qos(factory_qos);


    // CREATE THE COMMON READER ATTRIBUTES
    qos_ = DATAREADER_QOS_DEFAULT;
    // qos_.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    // qos_.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    // qos_.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    // qos_.history().depth = 30;
    // qos_.resource_limits().max_samples = 50;
    // qos_.resource_limits().allocated_samples = 20;
    mp_subscriber = mp_participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
    if (mp_subscriber == nullptr)
    {
        return;
    }
    mp_subscriber->enable();

}

EchoTool::~EchoTool()
{
    for (const auto& it : topics_)
    {
        mp_subscriber->delete_datareader(it.first);
        mp_participant->delete_topic(it.second);
    }
    if (mp_subscriber != nullptr)
    {
        mp_participant->delete_subscriber(mp_subscriber);
    }

    DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
    topics_.clear();
    readers_.clear();
    datas_.clear();
}

void EchoTool::run()
{
    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}


void EchoTool::subscribe_to_topic(const std::string& topic_name)
{
    sub_topic_ = topic_name;
    sub_topic_list_.push_back(topic_name);
    std::cout << "Subscribed to topic " << topic_name << std::endl;
}



void EchoTool::SubListener::on_subscription_matched(
        eprosima::fastdds::dds::DataReader* reader,
        const eprosima::fastdds::dds::SubscriptionMatchedStatus& info)
{
    if (info.current_count_change == 1)
    {
        n_matched++;
        std::cout << "Subscriber matched" << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        n_matched--;
        std::cout << "Subscriber unmatched" << std::endl;
        auto itr = subscriber_->readers_.find(reader);
        if (itr != subscriber_->readers_.end())
        {
            subscriber_->readers_.erase(itr);
        }

        auto itd = subscriber_->datas_.find(reader);
        if (itd != subscriber_->datas_.end())
        {
            subscriber_->datas_.erase(itd);
        }

        if (subscriber_->mp_subscriber != nullptr)
        {
            subscriber_->mp_subscriber->delete_datareader(reader);
        }
    }
    else
    {
        std::cout << "Subscriber received an invalid value for SubscriptionMatchedStatus." << std::endl;
    }
}



void EchoTool::SubListener::on_data_available(
        eprosima::fastdds::dds::DataReader* reader)
{
    auto dit = subscriber_->datas_.find(reader);

    if (dit != subscriber_->datas_.end())
    {
        types::DynamicData_ptr data = dit->second;
        SampleInfo info;
        if (reader->take_next_sample(data.get(), &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.valid_data)
            {
                types::DynamicType_ptr type = subscriber_->readers_[reader];
                this->n_samples++;
                // std::cout << "Received data of type " << type->get_name() << std::endl;
                types::DynamicDataHelper::print(data);
                std::cout << "-------------------------------\n"  << std::endl;
            }
        }
    }
}

void EchoTool::internal_notify_type_object_(types::DynamicType_ptr dynamic_type)
{

    // // Create data containing Dynamic Type
    // auto data = std::make_unique<types::DynamicTypeData>();
    // data->dynamic_type = dynamic_type; // TODO: add constructor with param
}

void EchoTool::SubListener::on_type_information_received(
        eprosima::fastdds::dds::DomainParticipant*,
        const eprosima::fastrtps::string_255 topic_name,
        const eprosima::fastrtps::string_255 type_name,
        const eprosima::fastrtps::types::TypeInformation& type_information)
{
    // std::cout << "Discovered type: " << type_name.to_string() << 
    //     " from topic " << topic_name.to_string() << std::endl;

    std::string type_name_ = type_name.to_string();
    const TypeIdentifier* type_identifier = nullptr;
    const TypeObject* type_object = nullptr;
    DynamicType_ptr dynamic_type(nullptr);

     // Check if complete identifier already present in factory
    type_identifier = TypeObjectFactory::get_instance()->get_type_identifier(type_name_, true);
    if (type_identifier)
    {
        type_object = TypeObjectFactory::get_instance()->get_type_object(type_name_, true);
    }

    // If complete not found, try with minimal
    if (!type_object)
    {
        type_identifier = TypeObjectFactory::get_instance()->get_type_identifier(type_name_, false);
        if (type_identifier)
        {
            type_object = TypeObjectFactory::get_instance()->get_type_object(type_name_, false);
        }
    }

    // Build dynamic type if type identifier and object found in factory
    if (type_identifier && type_object)
    {
        dynamic_type = TypeObjectFactory::get_instance()->build_dynamic_type(type_name_, type_identifier, type_object);
    }

    // // Request type object through TypeLookup if not present in factory, or if type building failed
    // if (!dynamic_type)
    // {
    //     std::function<void(const std::string& type_name, const DynamicType_ptr)> callback(
    //         [this]
    //             (const std::string&  type_name , const DynamicType_ptr type)
    //         {
    //             printf("---- type_name %s\n", type_name.c_str());
    //             this->internal_notify_type_object_(type);

    //             // if (topic_name != subscriber_->sub_topic_  )
    //             //     return;

    //             //CREATE THE TOPIC
    //             eprosima::fastdds::dds::Topic* topic = subscriber_->mp_participant->create_topic(
    //                 std::string(topic_name), name, TOPIC_QOS_DEFAULT);

    //             if (topic == nullptr)
    //             {
    //                 return;
    //             }

    //         });
    //     // Registering type and creating reader
    //     participant->register_remote_type(
    //         type_information,
    //         type_name_,
    //         callback);
    // } 
    // else
    // {
    //     internal_notify_type_object_(dynamic_type);
    // }
    std::function<void(const std::string&, const types::DynamicType_ptr)> callback =
            [this, topic_name](const std::string& name, const types::DynamicType_ptr type)
            {
                std::cout << "-----Discovered type: " << name << " from topic " << topic_name << std::endl;

                if (topic_name != subscriber_->sub_topic_  )
                    return;

                //CREATE THE TOPIC
                eprosima::fastdds::dds::Topic* topic = subscriber_->mp_participant->create_topic(
                    std::string(topic_name), name, TOPIC_QOS_DEFAULT);

                if (topic == nullptr)
                {
                    return;
                }

                StatusMask sub_mask = StatusMask::subscription_matched() << StatusMask::data_available();
                DataReader* reader = subscriber_->mp_subscriber->create_datareader(
                    topic,
                    subscriber_->qos_,
                    &subscriber_->m_listener,
                    sub_mask);

                if (type == nullptr)
                {
                    const types::TypeIdentifier* ident =
                            types::TypeObjectFactory::get_instance()->get_type_identifier_trying_complete(name);

                    if (nullptr != ident)
                    {
                        const types::TypeObject* obj =
                                types::TypeObjectFactory::get_instance()->get_type_object(ident);

                        types::DynamicType_ptr dyn_type =
                                types::TypeObjectFactory::get_instance()->build_dynamic_type(name, ident, obj);

                        if (nullptr != dyn_type)
                        {
                            subscriber_->readers_[reader] = dyn_type;
                            types::DynamicData_ptr data(
                                types::DynamicDataFactory::get_instance()->create_data(dyn_type));
                            subscriber_->datas_[reader] = data;
                        }
                        else
                        {
                            std::cout << "ERROR: DynamicType cannot be created for type: " << name << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "ERROR: TypeIdentifier cannot be retrieved for type: " << name << std::endl;
                    }
                }
                else
                {
                    subscriber_->topics_[reader] = topic;
                    subscriber_->readers_[reader] = type;
                    types::DynamicData_ptr data(types::DynamicDataFactory::get_instance()->create_data(type));
                    subscriber_->datas_[reader] = data;
                }
            };

    subscriber_->mp_participant->register_remote_type(
        type_information,
        type_name.to_string(),
        callback);
}



