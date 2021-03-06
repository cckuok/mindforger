/*
 memory.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef M8R_MEMORY_H_
#define M8R_MEMORY_H_

#include <vector>
#include <map>

#include "../debug.h"
#include "../exceptions.h"
#include "../mind/ontology/ontology.h"
#include "../config/configuration.h"
#include "../repository_indexer.h"
#include "../representations/markdown/markdown_document.h"
#include "../representations/markdown/markdown_outline_representation.h"
#include "../model/outline.h"
#include "../model/note.h"
#include "../model/stencil.h"
#include "../model/tag.h"
#include "../model/resource_types.h"
#include "../persistence/persistence.h"
#include "../persistence/filesystem_persistence.h"
#include "aspect/mind_scope_aspect.h"

namespace m8r {

class Memory
{
private:
    /**
     * @brief Indicates whether Mind learned a repository.
     */
    bool aware;

    /**      
     * @brief Cache outlines in memory.
     *
     * If FALSE, then outlines are always loaded from
     * filesystem on get(), else keep already loaded outline
     * ASTs in memory.
     */
    bool cache;

    Configuration& config;    
    RepositoryIndexer repositoryIndexer;
    Ontology ontology;
    MarkdownOutlineRepresentation representation;
    Persistence* persistence;
    MindScopeAspect* mindScope;

    std::vector<Outline*> outlines;
    std::vector<Note*> notes;
    std::vector<Stencil*> outlineStencils;
    std::vector<Stencil*> noteStencils;

    std::vector<Outline*> limboOutlines;

    // IMPROVE unordered_map
    std::map<std::string,Outline*> outlinesMap;

public:
    explicit Memory(Configuration& configuration);
    Memory(const Memory&) = delete;
    Memory(const Memory&&) = delete;
    Memory& operator=(const Memory&) = delete;
    Memory& operator=(const Memory&&) = delete;
    virtual ~Memory();

    void setMindScope(MindScopeAspect* mindScopeAspect) { mindScope = mindScopeAspect; }

    /**
     * @brief Learn repository content.
     */
    void learn();
    bool isAware() { return aware; }

    /**
     * @brief Forget everything.
     */
    void amnesia();

    std::vector<Stencil*>& getStencils(ResourceType type=ResourceType::OUTLINE);

    /**
     * @brief Create Outline from stencil, but don't learn it yet.
     */
    Outline* createOutline(Stencil* stencil);
    /**
     * @brief Create Note from stencil, but don't learn it yet.
     */
    Note* createNote(Stencil* stencil);

    /**
     * @brief Remember known Outline by saving it.
     */
    void remember(const std::string& outlineKey);

    /**
     * @brief Remember new Outline.
     */
    void remember(Outline* outline);

    /**
     * @brief Forget Outline.
     */
    void forget(Outline* outline);

    /**
     * @brief Get Ontology.
     * @return Ontology
     */
    Ontology& getOntology() { return ontology; }

    /**
     * @brief Get the number of outlines.
     */
    unsigned getOutlinesCount() const;

    /**
     * @brief Get the size of outline MDs in bytes.
     */
    unsigned int getOutlineMarkdownsSize() const;

    /**
     * @brief Get the number of notes of all outlines.
     */
    unsigned getNotesCount() const;

    std::string createOutlineKey(const std::string* name);
    std::string createLimboKey(const std::string* name);

    /**
     * @brief Get list of outlines.
     *
     * Get list of outlines - name/description/metadata presence
     * is guaranteed, while AST is not.
     */
    const std::vector<Outline*>& getOutlines() const;

    /**
     * @brief Get full outline.
     *
     * Get outline including AST - if Outline contains only name/description/metadata,
     * then AST is loaded and full outline returned.
     */
    Outline* getOutline(const std::string &key);

    /**
     * @brief Get notes of all outlines.
     */
    void getAllNotes(std::vector<Note*>& notes, bool sortByRead=false) const;

    /*
     * UTILS
     */

    void sortByName(std::vector<Outline*>& sorted) const;
    void sortByRead(std::vector<Note*>& sorted) const;
    RepositoryIndexer& getRepositoryIndexer() { return repositoryIndexer; }

private:
    const OutlineType* toOutlineType(const MarkdownAstSectionMetadata&);

};

} /* namespace */

#endif /* M8R_MEMORY_H_ */
