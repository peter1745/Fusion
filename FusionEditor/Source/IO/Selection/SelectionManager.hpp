#pragma once

#include <Fusion/Core/UUID.hpp>
#include <Fusion/Memory/Shared.hpp>
#include <Fusion/STL/Concepts.hpp>
#include <Fusion/STL/FunctionList.hpp>

#include <Fusion/World/Actor.hpp>

#include <unordered_map>

namespace FusionEditor {

	template<Fusion::Comparable TSelection, Fusion::Hashable TKey = Fusion::UUID>
	class SelectionManager : public Fusion::SharedObject
	{
	public:
		void Select(const TKey& InKey, const TSelection& InValue)
		{
			if (m_SelectionMap.find(InKey) != m_SelectionMap.end())
			{
				// Already selected
				FUSION_CLIENT_WARN("Attempting to select object with a key that's already in use: {}", InKey);
				return;
			}

			m_SelectionMap[InKey] = InValue;
			m_OnSelectedCallbacks.Invoke(InValue);
		}

		bool IsSelected(const TKey& InKey) const { return m_SelectionMap.find(InKey) != m_SelectionMap.end(); }
		bool IsSelected(const TSelection& InValue) const
		{
			for (auto Iter = m_SelectionMap.begin(); Iter != m_SelectionMap.end(); Iter++)
			{
				if (InValue == Iter->second)
					return true;
			}

			return false;
		}

		void Deselect(const TKey& InKey)
		{
			if (m_SelectionMap.find(InKey) == m_SelectionMap.end())
			{
				// Already selected
				FUSION_CLIENT_WARN("Attempting to deselect object with a key that hasn't been selected: {}", InKey);
				return;
			}

			m_OnDeselectedCallbacks.Invoke(m_SelectionMap.at(InKey));
			m_SelectionMap.erase(InKey);
		}

		void Deselect(const TSelection& InValue)
		{
			for (auto Iter = m_SelectionMap.begin(); Iter != m_SelectionMap.end(); Iter++)
			{
				if (InValue == Iter->second)
				{
					m_OnDeselectedCallbacks.Invoke(Iter->second);
					m_SelectionMap.erase(Iter);
					break;
				}
			}
		}

		void DeselectAll()
		{
			for (const auto& [Key, Value] : m_SelectionMap)
				m_OnDeselectedCallbacks.Invoke(Value);

			m_SelectionMap.clear();
		}

		const auto& All() const { return m_SelectionMap; }

		void AddSelectionCallback(const std::function<void(const TSelection&)>& InCallback)
		{
			m_OnSelectedCallbacks.AddFunction(InCallback);
		}

		void AddDeselectionCallback(const std::function<void(const TSelection&)>& InCallback)
		{
			m_OnDeselectedCallbacks.AddFunction(InCallback);
		}

	private:
		std::unordered_map<TKey, TSelection> m_SelectionMap;
		Fusion::FunctionList<void(const TSelection&)> m_OnSelectedCallbacks;
		Fusion::FunctionList<void(const TSelection&)> m_OnDeselectedCallbacks;
	};

	using ActorSelectionManager = Fusion::Shared<SelectionManager<Fusion::SharedActor>>;

}
