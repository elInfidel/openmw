#include "creatureanimation.hpp"

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreSubEntity.h>

#include "renderconst.hpp"

#include "../mwbase/world.hpp"

using namespace Ogre;
using namespace NifOgre;
namespace MWRender{

CreatureAnimation::~CreatureAnimation()
{
}

CreatureAnimation::CreatureAnimation(const MWWorld::Ptr& ptr, OEngine::Render::OgreRenderer& _rend): Animation(_rend)
{
    mInsert = ptr.getRefData().getBaseNode();
    MWWorld::LiveCellRef<ESM::Creature> *ref = ptr.get<ESM::Creature>();

    assert (ref->base != NULL);
    if(!ref->base->model.empty())
    {
        std::string mesh = "meshes\\" + ref->base->model;

        NifOgre::EntityList entities = NifOgre::NIFLoader::createEntities(mInsert, mesh);
        mBase = entities.mEntities;
        for(size_t i = 0;i < mBase.size();i++)
        {
            mBase[i]->setVisibilityFlags(RV_Actors);

            bool transparent = false;
            for (unsigned int j=0;j < mBase[i]->getNumSubEntities() && !transparent; ++j)
            {
                Ogre::MaterialPtr mat = mBase[i]->getSubEntity(j)->getMaterial();
                Ogre::Material::TechniqueIterator techIt = mat->getTechniqueIterator();
                while (techIt.hasMoreElements() && !transparent)
                {
                    Ogre::Technique* tech = techIt.getNext();
                    Ogre::Technique::PassIterator passIt = tech->getPassIterator();
                    while (passIt.hasMoreElements() && !transparent)
                    {
                        Ogre::Pass* pass = passIt.getNext();

                        if (pass->getDepthWriteEnabled() == false)
                            transparent = true;
                    }
                }
            }
            mBase[i]->setRenderQueueGroup(transparent ? RQG_Alpha : RQG_Main);
        }
    }
}

void CreatureAnimation::runAnimation(float timepassed)
{
    if(mAnimate > 0)
    {
        //Add the amount of time passed to time

        //Handle the animation transforms dependent on time

        //Handle the shapes dependent on animation transforms
        mTime += timepassed;
        if(mTime >= mStopTime)
        {
            mAnimate--;
            //std::cout << "Stopping the animation\n";
            if(mAnimate == 0)
                mTime = mStopTime;
            else
                mTime = mStartTime + (mTime - mStopTime);
        }

        handleAnimationTransforms();
    }
}

}
