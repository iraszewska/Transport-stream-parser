#include "tsTransportStream.h"
#include <iostream>

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================

/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
  //write me
  m_SB = 0;
  m_E = 0;
  m_S = 0;
  m_T = 0;
  m_PID = 0;
  m_TSC = 0;
  m_AFC = 0;
  m_CC = 0;
}

/**
  @brief Parse all TS packet header fields
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure) 
 */
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
  //i`m empty
  if(Input == nullptr){
    return -1;
  }

  // polaczenie 4 pierwszych bajtow do zmiennej (32-bit)
  //uint32_t *HP = (uint32_t*)Input;

  // zamiana kolejnosci bajtow - najbardziej znaczacy bajt pierwszy big endian
  int32_t Head = xSwapBytes32(*reinterpret_cast<const uint32_t*>(Input));

  m_SB = (Head & 0xff000000) >> 24;
  m_E = (Head & 0x800000) >> 23;
  m_S = (Head & 0x400000) >> 22;
  m_T = (Head & 0x200000) >> 21;
  m_PID = (Head & 0x1fff00) >> 8;
  m_TSC = (Head & 0xc0) >> 6;
  m_AFC = (Head & 0x30) >> 4;
  m_CC = Head & 0xf;

  return 4; // Number of parsed bytes
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  //print sth
  std::cout << "SB=" << (int)m_SB <<
    " E=" << (int)m_E <<
    " S=" << (int)m_S << 
    " m_T=" << (int)m_T << 
    " PID=" << (int)m_PID << 
    " TSC=" << (int)m_TSC << 
    " AFC=" << (int)m_AFC <<
    " CC=" << (int)m_CC << " ";
}

//=============================================================================================================================================================================

//=============================================================================================================================================================================
// xTS_AdaptationField
//=============================================================================================================================================================================

// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset(){
      // reset mandatory
      m_AdaptationFieldLength = 0;
      m_DC = 0;
      m_RA = 0;
      m_SP = 0;
      m_SF = 0;
      m_TP = 0;
      m_EX = 0;
      // reset optional fields
      m_PR = 0;
      m_OR = 0;
  }
/**
  @brief Parse adaptation field
  @param PacketBuffer is pointer to buffer containing TS packet
  @param AdaptationFieldControl is value of Adaptation Field Control field of
  corresponding TS packet header
  @return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl){
  //i`m empty
  if(!AdaptationFieldControl){
    return -1;
  }
  else{
    if(PacketBuffer == nullptr){
      return -1;
    }
  }

  uint8_t *AF = new uint8_t[2];
  for(int i = 0; i < 2; i++){
    AF[i] = (uint8_t)PacketBuffer[i+4];
  }

  //parsing
  //AF to wydzielona wczesniej czesc PacketBuffera
  uint16_t AField = xSwapBytes16(*(reinterpret_cast<const uint16_t*>(AF)));

  // parsing the mandatory fields
  // buffer 
  m_AdaptationFieldLength = AF[0];
  m_DC = (AField & 0x80) >> 7;
  m_RA = (AField & 0x40) >> 6;
  m_SP = (AField & 0x04) >> 5;

  m_PR = (AField & 0x10) >> 4;
  m_OR = (AField & 0x08) >> 3;   

  m_SF = (AField & 0x10) >> 2;
  m_TP = (AField & 0x02) >> 1;
  m_EX = AField & 0x01;

  //optional fields
  // int AFLenghtOptional = (int)m_AdaptationFieldLength;
  // if(AFLenghtOptional){
  //   uint16_t *AFOptional = new uint16_t[AFLenghtOptional];
  //   std::cout << AFLenghtOptional << ";;;" << std::endl;
  //   for(int i = 0; i < (AFLenghtOptional); i++){
  //     AFOptional[i] = (uint16_t)PacketBuffer[i+6];
  //     std::cout << (int)AFOptional[i] << std::endl;
  //   }

  //   uint64_t m_PCR = ((uint64_t)AFOptional[0] << 32) | ((uint64_t)AFOptional[1] << 16) | (uint64_t)AFOptional[2];
  //   std::cout << (int)m_PCR << std::endl;
  // }
}

/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const{
  //print sth
  std::cout << "AF: L=" << (int)m_AdaptationFieldLength <<
    " DC=" << (int)m_DC <<
    " RA=" << (int)m_RA << 
    " SP=" << (int)m_SP << 
    " PR=" << (int)m_PR << 
    " OR=" << (int)m_OR << 
    " SF=" << (int)m_SF <<
    " TP=" << (int)m_TP <<
    " EX=" << (int)m_EX << " ";
}

//=============================================================================================================================================================================
// xPES_PacketHeader
//=============================================================================================================================================================================

void xPES_Assembler::Init (int32_t PID){
  m_PID = PID;
};

void xPES_PacketHeader::Reset(){
  // reset 
  m_PacketStartCodePrefix = 0;
  m_StreamId = 0;
  m_PacketLength = 0;
}

int32_t xPES_PacketHeader::Parse(const uint8_t* PacketBuffer, uint32_t AFsize){
    //i`m empty
    if(PacketBuffer == nullptr){
      return -1;
    }
    //TO-DO
    uint8_t *PESH = new uint8_t[6];
    for(int i = 0; i < 6; i++){
      PESH[i] = (uint8_t)PacketBuffer[i+4+AFsize];
    }

    //parsing
    //PESH to wydzielona wczesniej czesc PacketBuffera
    // uint16_t PESHeader = xSwapBytes16(*(reinterpret_cast<const uint16_t*>(PESH)));

    // 24 bajty lacznie na prefix
    m_PacketStartCodePrefix = (PESH[0] << 16) | (PESH[1] << 8) | PESH[2];
    m_StreamId = PESH[3];
    //lacze dwa ostatnie bajty
    m_PacketLength = (PESH[4] << 8) | PESH[5];

    // jak jest 0xBE i 0xBF to nie ma extension
    if((int)m_StreamId != 190 and (int)m_StreamId != 191){
      uint8_t *PES_Extension = new uint8_t[3];
      for(int i = 0; i < 3; i++){
        PES_Extension[i] = (uint8_t)PacketBuffer[i+4+AFsize+6];
      }

      uint8_t PESE_DataLength = PES_Extension[2];
      // std::cout << "dlugosc naglowka: " << (int)PESE_DataLength << std::endl;
    }

    delete [] PESH;
    return 0;
}

void xPES_PacketHeader::Print() const{
  //print sth
  std::cout << "PES: PSCP=" << (int)m_PacketStartCodePrefix <<
    " SID=" << (int)m_StreamId <<
    " L=" << (int)m_PacketLength << std::endl;
}

//=============================================================================================================================================================================
// xPES_Assembler
//=============================================================================================================================================================================

void xPES_Assembler::xBufferReset (){
  m_BufferSize = 0;
  m_DataOffset = 0;
  m_Started = false;
}

void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t Size){
  
}

xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField){
  /*
    UnexpectedPID = 1,
    StreamPackedLost ,
    AssemblingStarted ,
    AssemblingContinue,
    AssemblingFinished,
  */
 
  //PESH
  m_PESH.Reset();
  if(PacketHeader->hasAFandPayload() && PacketHeader->getPUStartIndicator() == 1){
    uint32_t AFsize = AdaptationField->getNumBytes();
    m_PESH.Parse(TransportStreamPacket, AFsize);
  }
  else if(PacketHeader->hasAFandPayload()){
    uint32_t AFsize = AdaptationField->getNumBytes();
    m_PESH.Parse(TransportStreamPacket, AFsize);  
  }

  uint16_t num = 0;
  if(PacketHeader->hasAdaptationField()){
    num = AdaptationField->getNumBytes();
  }
  
  if(PacketHeader->hasPayload()){
    //Packet Header ma zawsze 8 bajtow
    m_DataOffset += 188 - num - 4;
    // std::cout << num << " " << m_DataOffset << std::endl;
  }

  /*
  PAYLOAD POINTER - WAZNE
  It gives the index after this byte at which the new payload unit starts.
  Any payload byte before the index is part of the previous payload unit. 
  */
  if(PacketHeader->getPUStartIndicator() == 1){
    m_Started = true;
  }

  if(m_Started){
    // m_BufferSize = m_PESH.getPacketLength();
    uint8_t size = 188;

    if(PacketHeader->hasAFandPayload()){
      uint8_t sizeAF = AdaptationField->getNumBytes();
      m_BufferSize = size-4-sizeAF;
      m_Buffer = new uint8_t[m_BufferSize];

      // do sprawdzenia czy ilosc bajtow, a w tym dodanych danych do tablicy m_Buffer sie zgadza
      // int temp = 0;

      for(int i = 0; i < m_BufferSize; i++){
        m_Buffer[i] = (uint8_t)TransportStreamPacket[i+4+AdaptationField->getNumBytes()];
        // temp++;
      }
      // std::cout << AdaptationField->getNumBytes() << "--num bytes" << std::endl;
      // std::cout << temp << "----" << std::endl;
    }
    else if(PacketHeader->hasPayload()){
      // Brak adaptation field, czyli 188 bajtow, +4 bo pomijamy header
      m_BufferSize = size-4;
      m_Buffer = new uint8_t[m_BufferSize];
      for(int i = 0; i < m_BufferSize; i++){
        m_Buffer[i] = (uint8_t)TransportStreamPacket[i+4];
      }
    }
  }


  if(PacketHeader->getPIDentifier() >= 0){
    Init(PacketHeader->getPIDentifier());
  }

  //weryfikacja PID
  if(PacketHeader->getPIDentifier() == 136){
    if(PacketHeader->getPUStartIndicator() == 1){
      xBufferReset();
      m_DataOffset += 188 - AdaptationField->getNumBytes() - 4;
      m_Started = true;
      // m_DataOffset = m_PESH.getPacketLength();
      m_LastContinuityCounter = PacketHeader->getContinuityCounter();
      return eResult::AssemblingStarted;
    }
    else if(m_Started){
    //weryfikacja ciaglosci CC
      if(PacketHeader->getContinuityCounter() == m_LastContinuityCounter+1){
          if(PacketHeader->getAFControl() == 3){
            m_Started = false;
            return eResult::AssemblingFinished;
          }
          m_LastContinuityCounter = PacketHeader->getContinuityCounter();
          return eResult::AssemblingContinue;
        }
      else{
        // m_Started = false;
        // std::cout << m_PESH.getPacketLength() << "--------" << std::endl;
        // return eResult::AssemblingFinished;
      }
    }
  }

  return eResult::UnexpectedPID;
};