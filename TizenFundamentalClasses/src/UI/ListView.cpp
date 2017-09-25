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
	auto newItemTemplate = this->itemTemplate->PackPayload(obj, this);
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
		ret = elm_genlist_item_append(widgetRoot, itemClass, data, nullptr, ELM_GENLIST_ITEM_NONE, OnItemClickedInternal, data);
	else
		ret = elm_genlist_item_insert_before(widgetRoot, itemClass, data, nullptr, itemBefore, ELM_GENLIST_ITEM_NONE, OnItemClickedInternal, data);
	return ret;
}

void ListView::RemoveListItem(Elm_Object_Item* item)
{
	elm_object_item_del(item);
}

LIBAPI
ListView::ListView(Evas_Object* parent) :
		WidgetBase(elm_genlist_add(parent))
{
	elm_genlist_highlight_mode_set(widgetRoot, EINA_FALSE);
	elm_genlist_select_mode_set(widgetRoot, ELM_OBJECT_SELECT_MODE_ALWAYS);

	eventItemRealized.Bind(widgetRoot, "realized");
}

LIBAPI
ListView::~ListView()
{

}

void TFC::UI::ListView::OnItemClickedInternal(void* data, Evas_Object* obj, void* dataItem)
{
	auto templatedItem = static_cast<ItemTemplateBase::ItemPayload*>(data);
	auto thiz = dynamic_cast<ListView*>(templatedItem->GetWidget());
	thiz->OnItemClicked(*static_cast<ObjectClass*>(templatedItem->GetItem()));
}

LIBAPI
void TFC::UI::ListView::UpdateItem(Elm_Object_Item* item)
{
	elm_genlist_item_update(item);
}

LIBAPI
void TFC::UI::ListView::ScrollToTop()
{
}

LIBAPI
void TFC::UI::ListView::ScrollToBottom()
{
}

LIBAPI
TFC::UI::GroupTemplateBase::ItemGroup::~ItemGroup()
{
}

LIBAPI
TFC::UI::GroupTemplateBase::~GroupTemplateBase()
{
}


LIBAPI
TFC::UI::GroupedListView::GroupedListView(Evas_Object* parent) :
		WidgetBase(elm_genlist_add(parent)),
		ListView(parent)
{
}

LIBAPI
void TFC::UI::GroupedListView::SetGroupTemplate(
		const std::shared_ptr<GroupTemplateBase>& it)
{
	this->groupTemplate = it;
}

LIBAPI
std::shared_ptr<GroupTemplateBase> TFC::UI::GroupedListView::GetGroupTemplate()
{
	return this->groupTemplate;
}

LIBAPI
Elm_Object_Item* TFC::UI::GroupedListView::AddItem(ObjectClass& obj,
		const void* baseAddress, Elm_Object_Item* itemBefore)
{
	// Get group info
	auto itemGroup = this->groupTemplate->GetGroupFor(obj);

	// Check if it is exist
	auto groupInfoIter = this->groupInfo.find(itemGroup);
	if(groupInfoIter == this->groupInfo.end())
	{
		// If it is not exist, create new group item
		auto insertResult = this->groupInfo.insert(std::make_pair(std::move(itemGroup), GroupInfo { this->groupTemplate->GetItemTemplate().PackPayload(*groupInfoIter->first, this) }));
		groupInfoIter = insertResult.first;

		Elm_Object_Item* itemBefore = nullptr;
		if(groupInfoIter != this->groupInfo.begin())
		{
			auto previousGroupInfo = groupInfoIter;
			--previousGroupInfo;

			itemBefore = previousGroupInfo->second.objectItem;
		}

		groupInfoIter->second.objectItem = this->AddListItem(&groupInfoIter->second.groupItemPayload, groupInfoIter->second.groupItemPayload, itemBefore);
	}

	++groupInfoIter->second.objectCount;
	indexByBaseAddress[baseAddress] = groupInfoIter;

	// Get templated item
	auto& templatedItem = this->GetTemplatedItem(obj, baseAddress);
	return this->AddListItemChild(&templatedItem, templatedItem, groupInfoIter->second.objectItem, itemBefore);
}

LIBAPI
void TFC::UI::GroupedListView::RemoveItem(Elm_Object_Item* item,
		const void* baseAddress)
{
	auto iter = this->indexByBaseAddress[baseAddress];
	--iter->second.objectCount;

	this->RemoveListItem(item);
	this->RemoveTemplatedItem(baseAddress);
	SelectorWidgetBase::RemoveItem(item, baseAddress);
	this->indexByBaseAddress.erase(baseAddress);

	if(iter->second.objectCount == 0)
	{
		this->RemoveListItem(iter->second.objectItem);
		this->groupInfo.erase(iter);
	}
}

LIBAPI
Elm_Object_Item* TFC::UI::GroupedListView::AddListItemChild(void* data,
		Elm_Gen_Item_Class* itemClass, Elm_Object_Item* parent,
		Elm_Object_Item* itemBefore)
{
	Elm_Object_Item* ret = nullptr;
	if(itemBefore == nullptr)
		ret = elm_genlist_item_append(widgetRoot, itemClass, data, parent, ELM_GENLIST_ITEM_NONE, OnItemClickedInternal, data);
	else
		ret = elm_genlist_item_insert_before(widgetRoot, itemClass, data, parent, itemBefore, ELM_GENLIST_ITEM_NONE, OnItemClickedInternal, data);
	return ret;
}
