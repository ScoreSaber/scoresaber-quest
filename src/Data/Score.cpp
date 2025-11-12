#include "Data/Score.hpp"
#include "Utils/StringUtils.hpp"
#include <beatsaber-hook/shared/utils/utils.h>
#include <paper2_scotland2/shared/string_convert.hpp>

namespace ScoreSaber::Data
{
    Score::Score(const rapidjson::Value&& value) : leaderboardPlayerInfo(value["leaderboardPlayerInfo"].GetObject())
    {
        auto idItr = value.FindMember("id");
        if (idItr != value.MemberEnd())
        {
            id = idItr->value.GetInt();
        }

        rank = value["rank"].GetInt();
        baseScore = value["baseScore"].GetInt();
        modifiedScore = value["modifiedScore"].GetInt();

        auto ppItr = value.FindMember("pp");
        if (ppItr != value.MemberEnd())
        {
            pp = ppItr->value.GetDouble();
        }

        auto weightItr = value.FindMember("weight");
        if (weightItr != value.MemberEnd())
        {
            weight = weightItr->value.GetDouble();
        }

        modifiers = value["modifiers"].GetString();
        multiplier = value["multiplier"].GetDouble();

        auto badCutsItr = value.FindMember("badCuts");
        if (badCutsItr != value.MemberEnd())
        {
            badCuts = badCutsItr->value.GetInt();
        }

        auto missedNotesItr = value.FindMember("missedNotes");
        if (missedNotesItr != value.MemberEnd())
        {
            missedNotes = missedNotesItr->value.GetInt();
        }

        auto maxComboItr = value.FindMember("maxCombo");
        if (maxComboItr != value.MemberEnd())
        {
            maxCombo = maxComboItr->value.GetInt();
        }

        fullCombo = value["fullCombo"].GetBool();

        auto hmdItr = value.FindMember("hmd");
        if (hmdItr != value.MemberEnd())
        {
            hmd = hmdItr->value.GetInt();
        }

        auto deviceHmdItr = value.FindMember("deviceHmd");
        if (deviceHmdItr != value.MemberEnd() && deviceHmdItr->value.IsString())
        {
            deviceHmd = deviceHmdItr->value.GetString();
        }

        auto deviceControllerLeftItr = value.FindMember("deviceControllerLeft");
        if (deviceControllerLeftItr != value.MemberEnd() && deviceControllerLeftItr->value.IsString())
        {
            deviceControllerLeft = deviceControllerLeftItr->value.GetString();
        }
        
        auto deviceControllerRightItr = value.FindMember("deviceControllerRight");
        if (deviceControllerRightItr != value.MemberEnd() && deviceControllerRightItr->value.IsString())
        {
            deviceControllerRight = deviceControllerRightItr->value.GetString();
        }

        hasReplay = value["hasReplay"].GetBool();

        auto timeSetItr = value.FindMember("timeSet");
        if (timeSetItr != value.MemberEnd())
        {
            timeSet = timeSetItr->value.GetString();
        }
    }

    Score::Score(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>&& value) : leaderboardPlayerInfo(value[u"leaderboardPlayerInfo"].GetObject())
    {
        auto idItr = value.FindMember(u"id");
        if (idItr != value.MemberEnd())
        {
            id = idItr->value.GetInt();
        }

        rank = value[u"rank"].GetInt();
        baseScore = value[u"baseScore"].GetInt();
        modifiedScore = value[u"modifiedScore"].GetInt();

        auto ppItr = value.FindMember(u"pp");
        if (ppItr != value.MemberEnd())
        {
            pp = ppItr->value.GetDouble();
        }

        auto weightItr = value.FindMember(u"weight");
        if (weightItr != value.MemberEnd())
        {
            weight = weightItr->value.GetDouble();
        }

        modifiers = Paper::StringConvert::from_utf16(value[u"modifiers"].GetString());
        multiplier = value[u"multiplier"].GetDouble();

        auto badCutsItr = value.FindMember(u"badCuts");
        if (badCutsItr != value.MemberEnd())
        {
            badCuts = badCutsItr->value.GetInt();
        }

        auto missedNotesItr = value.FindMember(u"missedNotes");
        if (missedNotesItr != value.MemberEnd())
        {
            missedNotes = missedNotesItr->value.GetInt();
        }

        auto maxComboItr = value.FindMember(u"maxCombo");
        if (maxComboItr != value.MemberEnd())
        {
            maxCombo = maxComboItr->value.GetInt();
        }

        fullCombo = value[u"fullCombo"].GetBool();

        auto hmdItr = value.FindMember(u"hmd");
        if (hmdItr != value.MemberEnd())
        {
            hmd = hmdItr->value.GetInt();
        }

        auto deviceHmdItr = value.FindMember(u"deviceHmd");
        if (deviceHmdItr != value.MemberEnd() && deviceHmdItr->value.IsString())
        {
            deviceHmd = Paper::StringConvert::from_utf16(deviceHmdItr->value.GetString());
        }

        auto deviceControllerLeftItr = value.FindMember(u"deviceControllerLeft");
        if (deviceControllerLeftItr != value.MemberEnd() && deviceControllerLeftItr->value.IsString())
        {
            deviceControllerLeft = Paper::StringConvert::from_utf16(deviceControllerLeftItr->value.GetString());
        }
        
        auto deviceControllerRightItr = value.FindMember(u"deviceControllerRight");
        if (deviceControllerRightItr != value.MemberEnd() && deviceControllerRightItr->value.IsString())
        {
            deviceControllerRight = Paper::StringConvert::from_utf16(deviceControllerRightItr->value.GetString());
        }

        hasReplay = value[u"hasReplay"].GetBool();

        auto timeSetItr = value.FindMember(u"timeSet");
        if (timeSetItr != value.MemberEnd())
        {
            timeSet = Paper::StringConvert::from_utf16(timeSetItr->value.GetString());
        }
    }

    Score::Score(rapidjson::GenericObject<true, rapidjson::Value> value) : leaderboardPlayerInfo(value["leaderboardPlayerInfo"].GetObject())
    {
        auto idItr = value.FindMember("id");
        if (idItr != value.MemberEnd())
        {
            id = idItr->value.GetInt();
        }

        rank = value["rank"].GetInt();
        baseScore = value["baseScore"].GetInt();
        modifiedScore = value["modifiedScore"].GetInt();

        auto ppItr = value.FindMember("pp");
        if (ppItr != value.MemberEnd())
        {
            pp = ppItr->value.GetDouble();
        }

        auto weightItr = value.FindMember("weight");
        if (weightItr != value.MemberEnd())
        {
            weight = weightItr->value.GetDouble();
        }

        modifiers = value["modifiers"].GetString();
        multiplier = value["multiplier"].GetDouble();

        auto badCutsItr = value.FindMember("badCuts");
        if (badCutsItr != value.MemberEnd())
        {
            badCuts = badCutsItr->value.GetInt();
        }

        auto missedNotesItr = value.FindMember("missedNotes");
        if (missedNotesItr != value.MemberEnd())
        {
            missedNotes = missedNotesItr->value.GetInt();
        }

        auto maxComboItr = value.FindMember("maxCombo");
        if (maxComboItr != value.MemberEnd())
        {
            maxCombo = maxComboItr->value.GetInt();
        }

        fullCombo = value["fullCombo"].GetBool();

        auto hmdItr = value.FindMember("hmd");
        if (hmdItr != value.MemberEnd())
        {
            hmd = hmdItr->value.GetInt();
        }

        auto deviceHmdItr = value.FindMember("deviceHmd");
        if (deviceHmdItr != value.MemberEnd() && deviceHmdItr->value.IsString())
        {
            deviceHmd = deviceHmdItr->value.GetString();
        }

        auto deviceControllerLeftItr = value.FindMember("deviceControllerLeft");
        if (deviceControllerLeftItr != value.MemberEnd() && deviceControllerLeftItr->value.IsString())
        {
            deviceControllerLeft = deviceControllerLeftItr->value.GetString();
        }
        
        auto deviceControllerRightItr = value.FindMember("deviceControllerRight");
        if (deviceControllerRightItr != value.MemberEnd() && deviceControllerRightItr->value.IsString())
        {
            deviceControllerRight = deviceControllerRightItr->value.GetString();
        }

        hasReplay = value["hasReplay"].GetBool();

        auto timeSetItr = value.FindMember("timeSet");
        if (timeSetItr != value.MemberEnd())
        {
            timeSet = timeSetItr->value.GetString();
        }
    }

    Score::Score(rapidjson::GenericObject<true, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>> value) : leaderboardPlayerInfo(value[u"leaderboardPlayerInfo"].GetObject())
    {
        auto idItr = value.FindMember(u"id");
        if (idItr != value.MemberEnd())
        {
            id = idItr->value.GetInt();
        }

        rank = value[u"rank"].GetInt();
        baseScore = value[u"baseScore"].GetInt();
        modifiedScore = value[u"modifiedScore"].GetInt();

        auto ppItr = value.FindMember(u"pp");
        if (ppItr != value.MemberEnd())
        {
            pp = ppItr->value.GetDouble();
        }

        auto weightItr = value.FindMember(u"weight");
        if (weightItr != value.MemberEnd())
        {
            weight = weightItr->value.GetDouble();
        }

        modifiers = Paper::StringConvert::from_utf16(value[u"modifiers"].GetString());
        multiplier = value[u"multiplier"].GetDouble();

        auto badCutsItr = value.FindMember(u"badCuts");
        if (badCutsItr != value.MemberEnd())
        {
            badCuts = badCutsItr->value.GetInt();
        }

        auto missedNotesItr = value.FindMember(u"missedNotes");
        if (missedNotesItr != value.MemberEnd())
        {
            missedNotes = missedNotesItr->value.GetInt();
        }

        auto maxComboItr = value.FindMember(u"maxCombo");
        if (maxComboItr != value.MemberEnd())
        {
            maxCombo = maxComboItr->value.GetInt();
        }

        fullCombo = value[u"fullCombo"].GetBool();

        auto hmdItr = value.FindMember(u"hmd");
        if (hmdItr != value.MemberEnd())
        {
            hmd = hmdItr->value.GetInt();
        }

        auto deviceHmdItr = value.FindMember(u"deviceHmd");
        if (deviceHmdItr != value.MemberEnd() && deviceHmdItr->value.IsString())
        {
            deviceHmd = Paper::StringConvert::from_utf16(deviceHmdItr->value.GetString());
        }

        auto deviceControllerLeftItr = value.FindMember(u"deviceControllerLeft");
        if (deviceControllerLeftItr != value.MemberEnd() && deviceControllerLeftItr->value.IsString())
        {
            deviceControllerLeft = Paper::StringConvert::from_utf16(deviceControllerLeftItr->value.GetString());
        }
        
        auto deviceControllerRightItr = value.FindMember(u"deviceControllerRight");
        if (deviceControllerRightItr != value.MemberEnd() && deviceControllerRightItr->value.IsString())
        {
            deviceControllerRight = Paper::StringConvert::from_utf16(deviceControllerRightItr->value.GetString());
        }

        hasReplay = value[u"hasReplay"].GetBool();

        auto timeSetItr = value.FindMember(u"timeSet");
        if (timeSetItr != value.MemberEnd())
        {
            timeSet = Paper::StringConvert::from_utf16(timeSetItr->value.GetString());
        }
    }
}