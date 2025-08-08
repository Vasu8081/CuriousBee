#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/network_message.h>
#include <string>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class youtube_resource : public network_message {
protected:
  // Properties
  std::string _resourceId;
  std::string _title;
  std::string _data;
  std::string _description;
public:
  // Constructor
  youtube_resource() {
    _data = "";
    _description = "";
    _msgType = message_type::youtubeResource;
    _resourceId = "";
    _title = "";
    _topic = "";
  }

  std::string getResourceId() const { return _resourceId; }
  void setResourceId(std::string value) { _resourceId = value; }

  std::string getTitle() const { return _title; }
  void setTitle(std::string value) { _title = value; }

  std::string getData() const { return _data; }
  void setData(std::string value) { _data = value; }

  std::string getDescription() const { return _description; }
  void setDescription(std::string value) { _description = value; }

  void toCapnp(curious::message::YoutubeResource::Builder& builder) const;
  static youtube_resource fromCapnp(const curious::message::YoutubeResource::Reader& reader);
  std::string serialize() const;
  static youtube_resource deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
