#ifndef __YS_LIFETIME_SPREADER_HPP__
#define __YS_LIFETIME_SPREADER_HPP__

#include <memory>
#include <vector>


template <typename base_type>
struct lifetime_spreader
{
	template <typename stored_type>
	stored_type& add_object(stored_type* _pointer)
	{
		managed_objects.push_back(std::unique_ptr<base_type>{_pointer});
		return *_pointer;
	}

	std::vector<std::unique_ptr<base_type>> managed_objects;
};


#endif // __YS_LIFETIME_SPREADER_HPP__
