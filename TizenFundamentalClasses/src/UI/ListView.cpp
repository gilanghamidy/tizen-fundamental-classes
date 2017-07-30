/*
 * ListView.cpp
 *
 *  Created on: Jul 27, 2017
 *      Author: gilang
 */

#include <TFC/UI/ListView.h>

using namespace TFC;
using namespace TFC::UI;

namespace {

class StringItemTemplate : public ItemTemplate<std::string>
{
public:
	StringItemTemplate() : ItemTemplate("default")
	{

	}
};

StringItemTemplate itemTemplate;

}

LIBAPI
ItemTemplateBase::ItemTemplateBase(const std::string& className) :
	itemClassName(className)
{
	itemClass.version = ELM_GENGRID_ITEM_CLASS_VERSION;
	itemClass.refcount = 0;
	itemClass.delete_me = EINA_FALSE;
	itemClass.homogeneous = EINA_FALSE;
	itemClass.item_style = itemClassName.c_str();
	itemClass.decorate_all_item_style = nullptr;
	itemClass.decorate_item_style = nullptr;

	itemClass.func.del = nullptr;
	itemClass.func.state_get = nullptr;
	itemClass.func.content_get = StaticGetContent;
	itemClass.func.text_get = StaticGetText;
}

char* ItemTemplateBase::StaticGetText(void* data, Evas_Object* obj,
		const char* part)
{
	auto payload = static_cast<ItemPayload*>(data);
	return payload->itemTemplate.GetTextInternal(payload->item, obj, part);
}

char* ItemTemplateBase::GetTextInternal(void* obj,
		Evas_Object* root, const char* part)
{
	return GetText(obj, root, part);
}

Evas_Object* ItemTemplateBase::GetContentInternal(void* obj,
		Evas_Object* root, const char* part) {
	return GetContent(obj, root, part);
}

Evas_Object* ItemTemplateBase::StaticGetContent(void* data,
		Evas_Object* obj, const char* part)
{
	auto payload = static_cast<ItemPayload*>(data);
	return payload->itemTemplate.GetContentInternal(payload->item, obj, part);
}

LIBAPI
void TemplatedContainerWidgetBase::SetItemTemplate(const std::shared_ptr<ItemTemplateBase>& it)
{
	this->itemTemplate = it;
}

LIBAPI
std::shared_ptr<ItemTemplateBase> TemplatedContainerWidgetBase::GetItemTemplate()
{
	return this->itemTemplate;
}

LIBAPI
ItemTemplateBase::ItemPayload& TemplatedContainerWidgetBase::GetTemplatedItem(ObjectClass& obj, const void* baseAddress)
{
	auto newItemTemplate = this->itemTemplate->PackPayload(obj);
	auto newStoredItem = this->itemPayloads.emplace(baseAddress, newItemTemplate);
	return newStoredItem.first->second;
}

LIBAPI
void TemplatedContainerWidgetBase::RemoveTemplatedItem(const void* baseAddress)
{
	this->itemPayloads.erase(baseAddress);
}

LIBAPI
Elm_Object_Item* ListViewBase::AddItem(ObjectClass& obj,const void* baseAddress, Elm_Object_Item* itemBefore)
{
	auto& templatedItem = this->GetTemplatedItem(obj, baseAddress);
	return this->AddListItem(&templatedItem, templatedItem, itemBefore);
}

LIBAPI
void ListViewBase::RemoveItem(Elm_Object_Item* item, const void* baseAddress) {
	this->RemoveListItem(item);
	this->RemoveTemplatedItem(baseAddress);
	SelectorWidgetBase::RemoveItem(item, baseAddress);
}

LIBAPI
Elm_Object_Item* ListView::AddListItem(void* data, Elm_Gen_Item_Class* itemClass, Elm_Object_Item* itemBefore)
{
	Elm_Object_Item* ret = nullptr;
	if(itemBefore == nullptr)
		ret = elm_genlist_item_append(widgetRoot, itemClass, data, nullptr, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	else
		ret = elm_genlist_item_insert_before(widgetRoot, itemClass, data, nullptr, itemBefore, ELM_GENLIST_ITEM_NONE, nullptr, nullptr);
	return ret;
}

void ListView::RemoveListItem(Elm_Object_Item* item)
{
	elm_object_item_del(item);
}

LIBAPI
ListView::ListView(Evas_Object* parent) :
		ContainerWidgetBase(elm_genlist_add(parent))
{
	elm_genlist_highlight_mode_set(widgetRoot, EINA_FALSE);
	elm_genlist_select_mode_set(widgetRoot, ELM_OBJECT_SELECT_MODE_ALWAYS);
}

LIBAPI
ListView::~ListView()
{
}
