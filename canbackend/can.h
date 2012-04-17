#include <stdint.h>

typedef struct
{
 // uint8_t sync[10];

  uint64_t timestamp;

  uint32_t stdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  uint32_t extId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  uint8_t ide;     /*!< Specifies the type of identifier for the message that will be transmitted.
                        This parameter can be a value of @ref CAN_identifier_type */

  uint8_t rtr;     /*!< Specifies the type of frame for the message that will be transmitted.
                        This parameter can be a value of @ref CAN_remote_transmission_request */

  uint8_t dlc;     /*!< Specifies the length of the frame that will be transmitted.
                        This parameter can be a value between 0 to 8 */

  uint8_t data[8]; /*!< Contains the data to be transmitted. It ranges from 0 to 0xFF. */
} CanMsg;

