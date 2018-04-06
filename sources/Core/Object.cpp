#include "Object.hpp"


Object::Object(void)
{
	std::cout << "Default constructor of Object called" << std::endl;
	this->_trasform = ;
	this->_name = "";
	this->_flags = 0;
}

Object::Object(Object const & src)
{
	*this = src;
	std::cout << "Copy constructor called" << std::endl;
}

Object::~Object(void)
{
	std::cout << "Destructor of Object called" << std::endl;
}


Object &	Object::operator=(Object const & src)
{
	std::cout << "Assignment operator called" << std::endl;

	if (this != &src) {
		this->_trasform = src.GetTransform();
		this->_name = src.GetName();
		this->_flags = src.GetFlags();
	}
	return (*this);
}

Transform		Object::GetTransform(void) const { return (this->_trasform); }
void		Object::SetTransform(Transform tmp) { this->_trasform = tmp; }

std::string		Object::GetName(void) const { return (this->_name); }
void		Object::SetName(std::string tmp) { this->_name = tmp; }

int		Object::GetFlags(void) const { return (this->_flags); }
void		Object::SetFlags(int tmp) { this->_flags = tmp; }

std::ostream &	operator<<(std::ostream & o, Object const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
