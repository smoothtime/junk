/* ========================================================================
 
   ======================================================================== */

void
notes()
{
    if(glm::dot(glm::cross(TLM, TM), TO) < 0)
    {
        // check TL, TM, TLM, T
        if(glm::dot(glm::cross(TL, TLM), TO) >= 0)
        {
            //check TL, T
            if(glm::dot(TL, TO) >= 0)
            {
                // it's TL, TLxAOxTL
            }
            else
            {
                if(glm::dot(TM, TO) >= 0)
                {
                    // it's TM, TMxTOxTM
                }
                else
                {
                    // it's T, TO
                }
            }
        }
        else
        {
            // This is impossible
            //if(glm::dot(glm::cross(TLM, TM), TO) >= 0)
            //{
            //   
            //}

            // inside triangle
            // can't be negative TLM or else plane test would be different

            // it's +TLM

            // POINT OF CONCERN
            // does being on this side of TM with regard to the 2d plane defined by TLM
            // actually make being in the TMR triangle impossible?
            // pretty sure it does
        }
    }
    else 
    {
        // check TM, TR, TMR, T

        // check TR
        if(glm::dot(glm::cross(TMR, TR), TO) >= 0)
        {
            // line segment check TR
            if(glm::dot(TR, TO) >= 0)
            {
                // it's TR, TRxTOxTR
            }
            else
            {
                // it's T, TO
            }
        }
        else
        {
            // checkTM
            if(glm::dot(glm::cross(TM, TMR), TO) >= 0)
            {
                // line segment check TM
                if(glm::dot(TM, TO) >= 0)
                {
                    // it's TM, TMxTOxTM
                }
                else
                {
                    // it's T, TO
                }
            }
            else
            {
                // can only be positive face normal
                // it's TMR, +TMR
            }
        }
    }
}
