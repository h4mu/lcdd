/*
    Copyright (c) <year>, <copyright holder>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY <copyright holder> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "lcd.h"

LCD::LCD()
{
  CPhidgetTextLCD_create(&m_lcd);
  CPhidgetHandle handle = reinterpret_cast<CPhidgetHandle>(m_lcd);
  CPhidget_open(handle, -1);
  if(CPhidget_waitForAttachment(handle, 600000))
  {
    throw std::exception();
  }
  CPhidgetTextLCD_setCursorOn(m_lcd, false);
  CPhidgetTextLCD_setBacklight(m_lcd, true);
  CPhidgetTextLCD_setCursorBlink(m_lcd, false);
  CPhidgetTextLCD_getColumnCount(m_lcd, &m_cols);
  CPhidgetTextLCD_getRowCount(m_lcd, &m_rows);
}

void LCD::print(const std::string& str, int row, bool center) const
{
  if(row <= m_rows)
  {
    if(row < 0)
      row = 0;
    if(center && str.length() < m_cols)
    {
      std::string tmp((m_cols - str.length() + 1) / 2, ' ');
      tmp.append(str);
      CPhidgetTextLCD_setDisplayString(m_lcd, row, const_cast<char*>(tmp.c_str()));	// I LOL'd @ need 2 const_cast
    }
    else
      CPhidgetTextLCD_setDisplayString(m_lcd, row, const_cast<char*>(str.substr(0, m_cols).c_str()));
  }
}

void LCD::bufferedUpdate(const std::string& str, int row)
{
  if(row <= m_rows)
  {
    if(row < 0)
      row = 0;
    if(str.length() > m_cols + 1)
    {
      RowBuffer & rowBuffer = m_framebuffer[row];
      if(str == rowBuffer.first)
      {
	if(++rowBuffer.second >= str.length())
	  rowBuffer.second = 0;
	std::string tmp(str.substr(rowBuffer.second));
	tmp += str.substr(0, rowBuffer.second - 1);
	print(tmp, row);
      }
      else
      {
	rowBuffer.first = str;
	rowBuffer.second = 0;
	print(str, row);
      }
    }
    else
      print(str, row);
  }
}

LCD::~LCD()
{
  CPhidgetHandle handle = reinterpret_cast<CPhidgetHandle>(m_lcd);
  CPhidget_close(handle);
  CPhidget_delete(handle);
}