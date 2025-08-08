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

class youtube_blog : public network_message {
protected:
  // Properties
  std::string _blogId;
  std::string _title;
  std::string _slug;
  std::string _coverImageUrl;
  std::string _publishedDate;
  std::string _contentHtml;
public:
  // Constructor
  youtube_blog() {
    _blogId = "";
    _contentHtml = "";
    _coverImageUrl = "";
    _msgType = message_type::youtubeBlog;
    _publishedDate = "";
    _slug = "";
    _title = "";
    _topic = "";
  }

  std::string getBlogId() const { return _blogId; }
  void setBlogId(std::string value) { _blogId = value; }

  std::string getTitle() const { return _title; }
  void setTitle(std::string value) { _title = value; }

  std::string getSlug() const { return _slug; }
  void setSlug(std::string value) { _slug = value; }

  std::string getCoverImageUrl() const { return _coverImageUrl; }
  void setCoverImageUrl(std::string value) { _coverImageUrl = value; }

  std::string getPublishedDate() const { return _publishedDate; }
  void setPublishedDate(std::string value) { _publishedDate = value; }

  std::string getContentHtml() const { return _contentHtml; }
  void setContentHtml(std::string value) { _contentHtml = value; }

  void toCapnp(curious::message::YoutubeBlog::Builder& builder) const;
  static youtube_blog fromCapnp(const curious::message::YoutubeBlog::Reader& reader);
  std::string serialize() const;
  static youtube_blog deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
