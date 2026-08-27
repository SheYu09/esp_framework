#include "Framework.h"
#include "Module.h"
#include "Rtc.h"
#include "Http.h"
#include "Util.h"

uint16_t Framework::rebootCount = 0;

void Framework::tickerPerSecondDo()
{
    perSecond++;
    if (perSecond == 30)
    {
        Rtc::rtcReboot.fast_reboot_count = 0;
        Rtc::rtcRebootSave();
    }
    if (rebootCount == 3)
    {
        return;
    }
    Rtc::perSecondDo();

    Config::perSecondDo();
    module->perSecondDo();
}

void Framework::one(unsigned long baud)
{
    Rtc::rtcRebootLoad();
    Rtc::rtcReboot.fast_reboot_count++;
    Rtc::rtcRebootSave();
    rebootCount = Rtc::rtcReboot.fast_reboot_count > BOOT_LOOP_OFFSET ? Rtc::rtcReboot.fast_reboot_count - BOOT_LOOP_OFFSET : 0;

    Serial.begin(baud);
    globalConfig.debug.type = 1;
}

void Framework::setup()
{
    Debug::AddError(PSTR("---------------------  v%s  %s  -------------------"), module->getModuleVersion().c_str(), Rtc::GetBuildDateAndTime().c_str());
    if (rebootCount == 1)
    {
        Config::readConfig();
        module->resetConfig();
    }
    else if (rebootCount == 2)
    {
        Config::readConfig();
        module->resetConfig();
    }
    else
    {
        Config::readConfig();
    }
    if (globalConfig.uid[0] != '\0')
    {
        strcpy(UID, globalConfig.uid);
    }
    else
    {
        uint8_t mac[6];
        wifi_get_macaddr(STATION_IF, mac);
        sprintf(UID, "%s_%02x%02x%02x", module->getModuleName().c_str(), mac[3], mac[4], mac[5]);
    }
    Util::strlowr(UID);

    Debug::AddInfo(PSTR("UID: %s"), UID);
    // Debug::AddInfo(PSTR("Config Len: %d"), GlobalConfigMessage_size + 6);

    //Config::resetConfig();
    if (rebootCount == 3)
    {
        module = NULL;

        tickerPerSecond = new Ticker();
        tickerPerSecond->attach(1, tickerPerSecondDo);

        Http::begin();
        Wifi::connectWifi();
    }
    else
    {
        module->init();
        tickerPerSecond = new Ticker();
        tickerPerSecond->attach(1, tickerPerSecondDo);
        Http::begin();
        Wifi::connectWifi();
        Rtc::init();
    }
}

void Framework::loop()
{
    if (rebootCount == 3)
    {
        Wifi::loop();
        Http::loop();
    }
    else
    {
        yield();
        Led::loop();
        yield();
        module->loop();
        yield();
        Wifi::loop();
        yield();
        Http::loop();
        yield();
        Rtc::loop();
    }
}
