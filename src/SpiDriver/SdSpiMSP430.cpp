/**
 * Copyright (c) 2011-2018 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Modified by PSA for MSP430 eUSCI-based SPI.
 * Patrick Allison (allison.122@osu.edu) - 2020
 * These modifications use eUSCI B0 and 2 DMA channels.
 * The 2 DMA channels are needed because in order to
 * receive SPI, we have to transmit stuff, so
 * we do a DMA transfer from a fixed dummy address
 * to the SPI transmit register, and then when the
 * receive DMA is triggered it copies the received
 * data.
 */
#ifdef ENERGIA
#include <Energia.h>
#if defined(__MSP430_HAS_EUSCI_B0__)
#include "SdSpiDriver.h"
#include <SPI.h>
/** Set SPI options for access to SD/SDHC cards.
 *
 * \param[in] divisor SCK clock divider relative to the APB1 or APB2 clock.
 */
void SdSpiAltDriver::activate() {
  // no idea  
}
//------------------------------------------------------------------------------
/** Initialize the SPI bus.
 *
 * \param[in] chipSelectPin SD card chip select pin.
 */
void SdSpiAltDriver::begin(uint8_t csPin) {
  m_csPin = csPin;
  pinMode(m_csPin, OUTPUT);
  digitalWrite(m_csPin, HIGH);
  SPI.begin(csPin);
}
//------------------------------------------------------------------------------
/**
 * End SPI transaction.
 */
void SdSpiAltDriver::deactivate() {
  // no idea
  //  SPI.endTransaction();
}
//------------------------------------------------------------------------------
/** Receive a byte.
 *
 * \return The byte.
 */
uint8_t SdSpiAltDriver::receive() {
  uint8_t rx;
  rx = SPI.transfer(0xFF);
  return rx;
}
//------------------------------------------------------------------------------
/** Receive multiple bytes.
 *
 * \param[out] buf Buffer to receive the data.
 * \param[in] n Number of bytes to receive.
 *
 * \return Zero for no error or nonzero error code.
 */
// This has to be a real valid location in memory.
uint8_t spi_bulk_dummy_tx;

uint8_t SdSpiAltDriver::receive(uint8_t* buf, size_t n) {
  while (UCB0STATW & UCBUSY);
  spi_bulk_dummy_tx = 0xFF;
  DMA0CTL = 0;
  DMA1CTL = 0;
  DMACTL0 = (DMA0TSEL__UCB0RXIFG0 | DMA1TSEL__UCB0TXIFG0);
  DMA0SA = (long unsigned int) &UCB0RXBUF;
  DMA1DA = (long unsigned int) &UCB0TXBUF;
  DMA0SZ = n;
  DMA1SZ = n;
  DMA1SA = (long unsigned int) &spi_bulk_dummy_tx;
  DMA0DA = (long unsigned int) buf;
  UCB0IFG = 0;
  DMA0CTL = (DMADT_0 | DMASRCINCR_0 | DMADSTINCR_3 | DMASBDB | DMAEN);
  DMA1CTL = (DMADT_0 | DMASRCINCR_0 | DMADSTINCR_0 | DMASBDB | DMAEN);
  UCB0IFG = UCTXIFG;
  while (!(DMA0CTL & DMAIFG));
  while (!(DMA1CTL & DMAIFG));
  return 0;  
}
//------------------------------------------------------------------------------
/** Send a byte.
 *
 * \param[in] b Byte to send
 */
void SdSpiAltDriver::send(uint8_t b) {
  SPI.transfer(b);
}
//------------------------------------------------------------------------------
/** Send multiple bytes.
 *
 * \param[in] buf Buffer for data to be sent.
 * \param[in] n Number of bytes to send.
 */
uint8_t spi_bulk_dummy_rx;
void SdSpiAltDriver::send(const uint8_t* buf , size_t n) {
  while (UCB0STATW & UCBUSY);
  DMA0CTL = 0;
  DMA1CTL = 0;
  DMACTL0 = (DMA0TSEL__UCB0RXIFG0 | DMA1TSEL__UCB0TXIFG0);
  DMA0SA = (long unsigned int) &UCB0RXBUF;
  DMA1DA = (long unsigned int) &UCB0TXBUF;
  DMA0SZ = n;
  DMA1SZ = n;
  DMA1SA = (long unsigned int) buf;
  DMA0DA = (long unsigned int) &spi_bulk_dummy_rx;
  UCB0IFG = 0;
  DMA0CTL = (DMADT_0 | DMASRCINCR_0 | DMADSTINCR_3 | DMASBDB | DMAEN);
  DMA1CTL = (DMADT_0 | DMASRCINCR_0 | DMADSTINCR_0 | DMASBDB | DMAEN);
  UCB0IFG = UCTXIFG;
  while (!(DMA0CTL & DMAIFG));
  while (!(DMA1CTL & DMAIFG));
}
#endif // defined(__MSP430_HAS_EUSCI_B0__)
#endif // ENERGIA
