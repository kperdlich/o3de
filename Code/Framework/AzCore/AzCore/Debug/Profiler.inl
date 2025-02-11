/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Interface/Interface.h>

namespace AZ::Debug
{
    namespace Platform
    {
        template<typename... T>
        void BeginProfileRegion(Budget* budget, const char* eventName, T const&... args);
        void BeginProfileRegion(Budget* budget, const char* eventName);
        void EndProfileRegion(Budget* budget);
        template<typename T>
        void ReportCounter(const Budget* budget, const wchar_t* counterName, const T& value);
        void ReportProfileEvent(const Budget* budget, const char* eventName);
    } // namespace Platform

    template<typename... T>
    void ProfileScope::BeginRegion(
        [[maybe_unused]] Budget* budget, [[maybe_unused]] const char* eventName, [[maybe_unused]] T const&... args)
    {
    #if !defined(_RELEASE)
        if (budget)
        {
            Platform::BeginProfileRegion(budget, eventName, args...);

            budget->BeginProfileRegion();

            if (auto profiler = AZ::Interface<Profiler>::Get(); profiler)
            {
                profiler->BeginRegion(budget, eventName, sizeof...(T), args...);
            }
        }
    #endif // !defined(_RELEASE)
    }

    inline void ProfileScope::EndRegion([[maybe_unused]] Budget* budget)
    {
    #if !defined(_RELEASE)
        if (budget)
        {
            budget->EndProfileRegion();

            if (auto profiler = AZ::Interface<Profiler>::Get(); profiler)
            {
                profiler->EndRegion(budget);
            }

            Platform::EndProfileRegion(budget);
        }
    #endif // !defined(_RELEASE)
    }

    template<typename... T>
    ProfileScope::ProfileScope(Budget* budget, const char* eventName, T const&... args)
        : m_budget{ budget }
    {
        BeginRegion(budget, eventName, args...);
    }

    inline ProfileScope::~ProfileScope()
    {
        EndRegion(m_budget);
    }

    template<typename T>
    inline void Profiler::ReportCounter(
        [[maybe_unused]] const Budget* budget, [[maybe_unused]] const wchar_t* counterName,
        [[maybe_unused]] const T& value)
    {
#if !defined(_RELEASE)
        Platform::ReportCounter(budget, counterName, value);
#endif // !defined(_RELEASE)
    }
    inline void Profiler::ReportProfileEvent([[maybe_unused]] const Budget* budget,
        [[maybe_unused]] const char* eventName)
    {
#if !defined(_RELEASE)
        Platform::ReportProfileEvent(budget, eventName);
#endif // !defined(_RELEASE)
    }

} // namespace AZ::Debug

#include <AzCore/Debug/Profiler_Platform.inl>
