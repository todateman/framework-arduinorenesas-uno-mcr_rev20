/*
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef ARDUINOCORE_API_CAN_MSG_H_
#define ARDUINOCORE_API_CAN_MSG_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <inttypes.h>
#include <string.h>

#include "Print.h"
#include "Printable.h"
#include "Common.h"

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace arduino
{

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class CanMsg : public Printable
{
public:
  static uint8_t  constexpr MAX_DATA_LENGTH = 8;

  static uint32_t constexpr CAN_EFF_FLAG    = 0x80000000U;
  static uint32_t constexpr CAN_SFF_MASK    = 0x000007FFU; /* standard frame format (SFF) */
  static uint32_t constexpr CAN_EFF_MASK    = 0x1FFFFFFFU; /* extended frame format (EFF) */


  CanMsg(uint32_t const can_id, uint8_t const can_data_len, uint8_t const * can_data_ptr)
  : id{can_id}
  , data_length{min(can_data_len, MAX_DATA_LENGTH)}
  , data{0}
  {
    memcpy(data, can_data_ptr, data_length);
  }

  CanMsg() : CanMsg(0, 0, nullptr) { }

  CanMsg(CanMsg const & other)
  {
    this->id          = other.id;
    this->data_length = other.data_length;
    memcpy(this->data, other.data, this->data_length);
  }

  virtual ~CanMsg() { }

  CanMsg & operator = (CanMsg const & other)
  {
    if (this != &other)
    {
      this->id          = other.id;
      this->data_length = other.data_length;
      memcpy(this->data, other.data, this->data_length);
    }
    return (*this);
  }

  virtual size_t printTo(Print & p) const override
  {
    char buf[20] = {0};
    size_t len = 0;

    /* Print the header. */
    if (isStandardId())
      len = snprintf(buf, sizeof(buf), "[%03" PRIX32 "] (%d) : ", getStandardId(), data_length);
    else
      len = snprintf(buf, sizeof(buf), "[%08" PRIX32 "] (%d) : ", getExtendedId(), data_length);
    size_t n = p.write(buf, len);

    /* Print the data. */
    for (size_t d = 0; d < data_length; d++)
    {
      len = snprintf(buf, sizeof(buf), "%02X", data[d]);
      n += p.write(buf, len);
    }

    /* Wrap up. */
    return n;
  }


  uint32_t getStandardId() const {
    return (id & CAN_SFF_MASK);
  }
  uint32_t getExtendedId() const {
    return (id & CAN_EFF_MASK);
  }
  bool isStandardId() const {
    return ((id & CAN_EFF_FLAG) == 0);
  }
  bool isExtendedId() const {
    return ((id & CAN_EFF_FLAG) == CAN_EFF_FLAG);
  }


  /*
   * CAN ID semantics (mirroring definition by linux/can.h):
   * |- Bit   31 : frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
   * |- Bit   30 : reserved (future remote transmission request flag)
   * |- Bit   29 : reserved (future error frame flag)
   * |- Bit 0-28 : CAN identifier (11/29 bit)
   */
  uint32_t id;
  uint8_t  data_length;
  uint8_t  data[MAX_DATA_LENGTH];
};

/**************************************************************************************
 * FREE FUNCTIONS
 **************************************************************************************/

inline uint32_t CanStandardId(uint32_t const id) {
  return (id & CanMsg::CAN_SFF_MASK);
}

inline uint32_t CanExtendedId(uint32_t const id) {
  return (CanMsg::CAN_EFF_FLAG | (id & CanMsg::CAN_EFF_MASK));
}

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* arduino */

#endif /* ARDUINOCORE_API_CAN_MSG_H_ */
