#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config {

  bool xmlEnabled;

public:

  Config() {
    xmlEnabled = true;
  }

  bool isXmlEnabled() {
    return xmlEnabled;
  }

  void setXmlEnabled(bool enabled) {
    xmlEnabled = enabled;
  }
};


#endif /* _CONFIG_H_ */
