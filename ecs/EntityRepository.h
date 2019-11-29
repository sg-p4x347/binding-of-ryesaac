#pragma once
#include "pch.h"

#include "Component.h"

namespace ecs {
	// Forward declare iterator class
	template<typename RepoType, typename ... QueryTypes>
	class EntityIterator;

	template<typename ... CompTypes>
	class EntityRepository
	{
	public:
		// Give iterator access to m_components
		template<typename, typename...>
		friend class EntityIterator;
		EntityRepository();
		template<typename ... EntityComps>
		EntityID CreateEntity(EntityComps&&... components);
		void Remove(EntityID id);
		template<typename CompType>
		void Remove(EntityID id);
		template<typename First, typename Second, typename ... Rest>
		void Remove(EntityID id);
		static void Copy(EntityID id, EntityRepository<CompTypes...>& source, EntityRepository<CompTypes...>& destination);
		
		template<typename ... QueryTypes>
		EntityIterator<EntityRepository<CompTypes...>, QueryTypes...> GetIterator();
	private:
		//----------------------------------------------------------------
		// Push new components to the end of the component vectors
		template<typename Comp>
		void PushComponent(EntityID id, Comp&& component);
		template<typename First, typename Second, typename ... Rest>
		void PushComponent(EntityID id, First&& first, Second&& second, Rest&& ... rest);
		//----------------------------------------------------------------
		// Copy components from one repository to another
		template<typename CompType>
		static void Copy(EntityID id, EntityRepository<CompTypes...>& source, EntityRepository<CompTypes...>& destination);
		template<typename First, typename Second, typename ... Rest>
		static void Copy(EntityID id, EntityRepository<CompTypes...>& source, EntityRepository<CompTypes...>& destination);
	private:
		tuple<vector<CompTypes>...> m_components;
		EntityID m_nextId;
	};

	template<typename ...CompTypes>
	inline EntityRepository<CompTypes...>::EntityRepository() : m_nextId(1)
	{
	}

	template<typename ...CompTypes>
	inline void EntityRepository<CompTypes...>::Remove(EntityID id)
	{
		Remove<CompTypes...>(id);
	}
	template<typename ...CompTypes>
	inline void EntityRepository<CompTypes...>::Copy(EntityID id, EntityRepository<CompTypes...>& source, EntityRepository<CompTypes...>& destination)
	{
		Copy<CompTypes...>(id, source, destination);
		destination.m_nextId++;
	}
	template<typename ...CompTypes>
	template<typename CompType>
	void EntityRepository<CompTypes...>::Copy(EntityID id, EntityRepository<CompTypes...>& source, EntityRepository<CompTypes...>& destination) {
		auto& sourceVector = std::get<vector<CompType>>(source.m_components);
		for (int i = 0; i < sourceVector.size(); i++) {
			if (sourceVector[i].ID == id) {
				destination.PushComponent(destination.m_nextId,CompType(sourceVector[i]));
			}
			else if (sourceVector[i].ID > id) {
				return;
			}
		}
	}
	template<typename ...CompTypes>
	template<typename First, typename Second, typename ... Rest>
	void EntityRepository<CompTypes...>::Copy(EntityID id, EntityRepository<CompTypes...>& source, EntityRepository<CompTypes...>& destination) {
		Copy<First>(id, source, destination);
		Copy<Second, Rest...>(id, source, destination);
	}
	template<typename ...CompTypes>
	template<typename CompType>
	inline void EntityRepository<CompTypes...>::Remove(EntityID id)
	{
		auto& componentVector = std::get<vector<CompType>>(m_components);
		for (int i = 0; i < componentVector.size(); i++) {
			if (componentVector[i].ID == id) {
				componentVector.erase(componentVector.begin() + i);
			}
			else if (componentVector[i].ID > id) {
				return;
			}
		}
	}
	template<typename ...CompTypes>
	template<typename First, typename Second, typename ... Rest>
	inline void EntityRepository<CompTypes...>::Remove(EntityID id)
	{
		Remove<First>(id);
		Remove<Second, Rest...>(id);
	}

	template<typename ...CompTypes>
	template<typename ...EntityComps>
	inline EntityID EntityRepository<CompTypes...>::CreateEntity(EntityComps&&...components)
	{
		EntityID id = m_nextId++;
		PushComponent(id, std::move(components)...);
		return id;
	}

	template<typename ...CompTypes>
	template<typename ...QueryTypes>
	inline EntityIterator<EntityRepository<CompTypes...>, QueryTypes...> EntityRepository<CompTypes...>::GetIterator()
	{
		return EntityIterator<EntityRepository<CompTypes...>, QueryTypes...>(*this);
	}

	template<typename ...CompTypes>
	template<typename Comp>
	inline void EntityRepository<CompTypes...>::PushComponent(EntityID id, Comp&& component)
	{
		component.ID = id;
		std::get<vector<Comp>>(m_components).push_back(std::move(component));
	}

	template<typename ...CompTypes>
	template<typename First, typename Second, typename ...Rest>
	inline void EntityRepository<CompTypes...>::PushComponent(EntityID id, First&& first, Second&& second, Rest&& ...rest)
	{
		PushComponent(id, std::move(first));
		PushComponent(id, std::move(second), std::move(rest)...);
	}

	//----------------------------------------------------------------
	// Iterator

	template<typename RepoType, typename ... QueryTypes>
	class EntityIterator {
	public:
		EntityIterator(RepoType& repository);
		template<typename CompType>
		CompType& Get() const;
		const EntityIterator& operator*() const;
		bool operator==(const EntityIterator<RepoType, QueryTypes...>& other) const;
		bool operator!=(const EntityIterator<RepoType, QueryTypes...>& other) const;

		EntityIterator<RepoType, QueryTypes...> operator++(int);
		EntityIterator<RepoType, QueryTypes...>& operator++();

		EntityIterator<RepoType, QueryTypes...> begin();
		EntityIterator<RepoType, QueryTypes...> end();
		bool End();
	private:
		template<typename CompType>
		void Increment(EntityID entity);
		template<typename First, typename Second, typename ... Rest>
		void Increment(EntityID entity);
	private:
		tuple<pair<QueryTypes*, int32_t>...> m_componentIndices;
		bool m_end;
		RepoType& m_repository;
	};

	template<typename RepoType, typename ...QueryTypes>
	inline const EntityIterator<RepoType, QueryTypes...>& EntityIterator<RepoType, QueryTypes...>::operator*() const
	{
		return *this;
	}

	template<typename RepoType, typename ...QueryTypes>
	inline bool EntityIterator<RepoType, QueryTypes...>::operator==(const EntityIterator<RepoType, QueryTypes...>& other) const
	{
		return m_end && other.m_end;
	}

	template<typename RepoType, typename ...QueryTypes>
	inline bool EntityIterator<RepoType, QueryTypes...>::operator!=(const EntityIterator<RepoType, QueryTypes...>& other) const
	{
		return !(*this == other);
	}

	template<typename RepoType, typename ...QueryTypes>
	inline EntityIterator<RepoType, QueryTypes...> EntityIterator<RepoType, QueryTypes...>::operator++(int)
	{
		EntityIterator<QueryTypes...> copy(*this);
		++* this;
		return copy;
	}

	template<typename RepoType, typename ...QueryTypes>
	inline EntityIterator<RepoType, QueryTypes...>& EntityIterator<RepoType, QueryTypes...>::operator++()
	{
		if (!m_end) {
			auto& firstPair = std::get<0>(m_componentIndices);
			++firstPair.second;
			auto& firstCompVector = std::get<vector<std::remove_pointer<decltype(firstPair.first)>::type>>(m_repository.m_components);
			if (firstPair.second == firstCompVector.size()) {
				m_end = true;
			}
			else {
				EntityID entity = firstCompVector[firstPair.second].ID;
				Increment<QueryTypes...>(entity);
			}
		}
		return *this;
	}

	template<typename RepoType, typename ...QueryTypes>
	inline EntityIterator<RepoType, QueryTypes...> EntityIterator<RepoType, QueryTypes...>::begin()
	{
		return EntityIterator<RepoType, QueryTypes...>(*this);
	}

	template<typename RepoType, typename ...QueryTypes>
	inline EntityIterator<RepoType, QueryTypes...> EntityIterator<RepoType, QueryTypes...>::end()
	{
		auto end = EntityIterator<RepoType, QueryTypes...>(*this);
		end.m_end = true;
		return end;
	}

	template<typename RepoType, typename ...QueryTypes>
	inline bool EntityIterator<RepoType, QueryTypes...>::End()
	{
		return m_end;
	}

	template<typename RepoType, typename ...QueryTypes>
	inline EntityIterator<RepoType, QueryTypes...>::EntityIterator(RepoType& repository) : m_repository(repository), m_end(false)
	{
		auto& firstPair = std::get<0>(m_componentIndices).second = -1;
		++* this;
	}

	template<typename RepoType, typename ...QueryTypes>
	template<typename CompType>
	inline CompType& EntityIterator<RepoType, QueryTypes...>::Get() const
	{
		return *std::get<pair<CompType*, int32_t>>(m_componentIndices).first;
	}

	template<typename RepoType, typename ...QueryTypes>
	template<typename CompType>
	inline void EntityIterator<RepoType, QueryTypes...>::Increment(EntityID entity)
	{
		auto& componentVector = std::get<vector<CompType>>(m_repository.m_components);
		auto& currentIndex = std::get<pair<CompType*, int32_t>>(m_componentIndices);
		while (componentVector[currentIndex.second].ID < entity) {
			if (++currentIndex.second == componentVector.size()) {
				m_end = true;
				return;
			}
		}
		if (componentVector[currentIndex.second].ID > entity) {
			m_end = true;
			return;
		}
		currentIndex.first = &(componentVector[currentIndex.second]);
	}

	template<typename RepoType, typename ...QueryTypes>
	template<typename First, typename Second, typename ... Rest>
	inline void EntityIterator<RepoType, QueryTypes...>::Increment(EntityID entity)
	{
		Increment<First>(entity);
		if (!m_end) {
			Increment<Second, Rest...>(entity);
		}
	}
}