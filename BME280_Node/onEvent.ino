/*********************************
 * Event-Handler
 */

void onEvent(ev_t ev)
{
uint8_t i = 0;

  switch (ev)
  {
    #ifdef AUSGABE
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT")); 
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    #endif
    case EV_JOINED:
    #ifdef AUSGABE
      Serial.println(F("EV_JOINED"));
    #endif
    LMIC_setLinkCheckMode(0);
    for (i = 0; i < 20; i++)  // nach erfolgreichem Join wird 20x kurz geblinkt
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
    break;
    #ifdef AUSGABE
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));     
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
      #endif
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      Serial.println();  // AUSGABE
      if (LMIC.txrxFlags & TXRX_ACK)
      #ifdef AUSGABE
        Serial.println(F("Received ack"));
      if (LMIC.dataLen)
      {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      #endif
//nach Aussendung kurz blinken
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);

// Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    #ifdef AUSGABE
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE"));
      break;
    #endif  
    default:
    #ifdef AUSGABE
      Serial.println(F("Unknown event"));
    #endif
      break;
  }
}
