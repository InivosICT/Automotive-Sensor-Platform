#include "PayloadBuilder.hpp"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string PayloadBuilder::build(const SensorData& data) const
{
    rapidjson::StringBuffer buffer;

    rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};

    writer.StartObject();

    writer.Key("vehicleId");
    writer.String(data.vehicleId.c_str());

    writer.Key("sensorId");
    writer.String(data.sensorId.c_str());

    writer.Key("sensorType");
    writer.String(data.sensorType.c_str());

    writer.Key("value");
    writer.Double(data.value);

    // writer.Key("sequenceNumber");
    // writer.Uint64(data.sequenceNumber);

    writer.Key("timestampMs");
    writer.Uint64(data.timestampMs);

    writer.EndObject();

    return std::string{buffer.GetString(),buffer.GetSize()};
}